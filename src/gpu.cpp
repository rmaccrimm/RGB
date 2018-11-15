#include "gpu.h"
#include "registers.h"
#include "interrupts.h"
#include "debug.h"
#include <iostream>
#include <iomanip>
#include <cassert>
#include <algorithm>

GPU::GPU(Memory *mem, GameWindow *win): 
    memory(mem), window(win), stat_reg(mem->io_registers[reg::STAT]), clock(0), line(0), mode(OAM)
{
    framebuffer.resize(256 * 256, 0); 
}

void GPU::step(unsigned int cpu_clock) 
{
    clock += cpu_clock;

    switch (mode)
    {
    case OAM:
        if (clock >= 80) {
            clock -= 80;
            change_mode(VRAM);
        }
        break;

    case VRAM:
        if (clock >= 172) {
            clock -= 172;
            change_mode(HBLANK);
        }
        break;

    case HBLANK:
        if (clock >= 204) {
            clock -= 204;
            increment_line();
            
            if (line == 143) {
                change_mode(VBLANK);
                // Draw screen
				set_bg_palette();
				if (memory->vram_updated) {
					build_framebuffer();
					memory->vram_updated = false;
				}
                window->draw_frame(
                    framebuffer.data(), memory->read(reg::SCROLLX), memory->read(reg::SCROLLY));
                memory->set_interrupt(interrupt::VBLANK_bit);
            } else {
                change_mode(OAM);
            }
        }
        break;

    case VBLANK:
        if (clock >= 456) {
            clock -= 456;
            increment_line();

            if (line == 154) {
                line = 0;
                change_mode(OAM);
                // Clear bit 0 of interrupt request
                u8 int_request = memory->read(reg::IF);
                memory->write(reg::IF, utils::reset(int_request, interrupt::VBLANK_bit));
            }
        }
        break;    
    }
    update_stat_register();
}

void GPU::update_stat_register()
{
    // set LCDSTAT interrupt request if internal signal goes from 0 to 1
    bool prev_sig = stat_irq_signal;
    u8 stat = memory->read(reg::STAT);

    u8 coincidence_enable = 1 << 6;
    u8 oam_enable = 1 << 5;
    u8 vblank_enable = 1 << 4;
    u8 hblank_enable = 1 << 3;
    u8 coincidence_set = 1 << 2;

    stat_irq_signal = ((stat & coincidence_enable) && (stat & coincidence_set)) ||
                      ((stat & hblank_enable) && mode == HBLANK) ||
                      ((stat & oam_enable) && mode == OAM) ||
                      ((stat & (vblank_enable | oam_enable)) && mode == VBLANK);
    
    if (!prev_sig && stat_irq_signal) {
        memory->set_interrupt(interrupt::LCDSTAT_bit);
    }
}

void GPU::change_mode(Mode m)
{
    mode = m;
    // TODO - If LCD is off, set to 0
    stat_reg.set((stat_reg.value() & ~3) | (int)mode);
}

void GPU::increment_line()
{
    line++;
    memory->write(reg::LY, line);
    u8 stat = utils::set_cond(stat_reg.value(), 2, memory->read(reg::LYC) == memory->read(reg::LY));
    stat_reg.set(stat);
}

void GPU::build_framebuffer()
{
    render_background();
    // render_sprites();
}

void GPU::set_bg_palette()
{
    u8 bgp = memory->read(reg::BGP);
    color_palette[0] = COLORS[bgp & 3];
    color_palette[1] = COLORS[(bgp >> 2) & 3];
    color_palette[2] = COLORS[(bgp >> 4) & 3];
    color_palette[3] = COLORS[(bgp >> 6) & 3];
}

// dest is a pointer to the first pixel in the framebuffer where the tile will be loaded
void GPU::read_tile(std::vector<u8>::iterator dest, u16 tile_addr)
{
    std::vector<u8>::iterator pix_data = memory->get_vram_ptr(tile_addr);

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            // two bytes per line, contain lsb and msb of color
            int byte_ind = 2 * (7 - j);
            u8 lsb = (pix_data[byte_ind] >> (7 - i)) & 1;
            u8 msb = (pix_data[byte_ind + 1] >> (7 - i)) & 1;
            int color = ((msb << 1) | lsb) & 3;
            dest[256 * j + i] = color_palette[color];
        }
    }
}

void GPU::render_background()
{
    u8 lcd_control = memory->read(reg::LCDC);
    u16 tile_map;
    u16 tile_data; 
    bool signed_map;

    if (lcd_control & BG_ENABLE) {
        if (lcd_control & BG_TILE_MAP_SELECT) {
            tile_map = TILE_MAP_1;
        }
        else {
            tile_map = TILE_MAP_0;
        }
        if (lcd_control & TILE_DATA_SELECT) {
            tile_data = TILE_DATA_1;
            signed_map = false;
        }
        else {
            tile_data = TILE_DATA_0;
            signed_map = true;
        }  

        for (int tile_i = 0; tile_i < 32; tile_i++) {
            for (int tile_j = 0; tile_j < 32; tile_j++) {

                // locate the tiles in memory
                int map_index = 32 * (tile_i % 32) + (tile_j % 32);

                // read the tile map and determine address of tile
                u16 tile_addr;
                if (signed_map) {
                    i8 tile_num = (i8)memory->read(tile_map + map_index);
                    tile_addr = tile_data + (16 * tile_num);
                }
                else {
                    u8 tile_num = memory->read(tile_map + map_index);
                    tile_addr = tile_data + (16 * tile_num);
                }

                int pixel_y = 256 - 8 * (tile_i + 1);
                int pixel_x = 8 * tile_j;
                int pixel_index = 256 * pixel_y + pixel_x;
                
                read_tile(framebuffer.begin() + pixel_index, tile_addr);
            }
        }        
    }
}

void GPU::render_window()
{

}

void GPU::render_sprites()
{
    // u8 lcd_control = memory->read(reg::LCDC);
    u8 scroll_y = memory->read(reg::SCROLLY);
    u8 scroll_x = memory->read(reg::SCROLLX);

    // bool enable_sprites = utils::bit(lcd_control, 1);
    // bool two_tile_sprites = utils::bit(lcd_control, 2);
    
    std::vector<u8>::iterator sprite_data = memory->get_vram_ptr(OAM_data);
    for (int i = 0; i < 40; i++) {
        int byte_ind = 4 * i;
        int ypos = sprite_data[byte_ind];
        int xpos = sprite_data[byte_ind + 1];

        if (xpos == 0 || xpos >= 168 || ypos == 0 || ypos >= 160) {
            continue;
        }
        ypos += scroll_y + 16;
        xpos += scroll_x + 8;

        int tile_num = sprite_data[byte_ind + 2];
        int flags = sprite_data[byte_ind + 3];

        // bool behind_bg = utils::bit(flags, 7);
        // bool flip_y = utils::bit(flags, 6);
        // bool flip_x = utils::bit(flags, 5);
        bool palette_num = utils::bit(flags, 4);

        u16 obp_addr[2] = { reg::OBP0, reg::OBP1 };
        u8 obp = memory->read(obp_addr[palette_num]);
        color_palette[0] = COLORS[obp & 3];
        color_palette[1] = COLORS[(obp >> 2) & 3];
        color_palette[2] = COLORS[(obp >> 4) & 3];
        color_palette[3] = COLORS[(obp >> 6) & 3];

        // position of lower left corner in framebuffer
        int pixel_index = 256 * (256 - ypos) + xpos;
        u16 tile_addr = TILE_DATA_1 + (16 * tile_num);

        read_tile(framebuffer.begin() + pixel_index, tile_addr);
    }
}

