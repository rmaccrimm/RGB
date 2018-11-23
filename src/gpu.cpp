#include "gpu.h"
#include "registers.h"
#include "interrupts.h"
#include "debug.h"
#include <iostream>
#include <iomanip>
#include <cassert>
#include <algorithm>

GPU::GPU(Memory *mem, GameWindow *win): 
    memory(mem), 
    window(win), 
    clock(0), 
    line(0),
    mode(OAM), 
    STAT_reg(mem->get_mem_reference(reg::STAT)),
    prev_cpu_clock(0)
{
    framebuffer.resize(256 * 256, 0);
    sprite_texture.resize(176 * 176 * 2, 0);
}

void GPU::step(unsigned int cpu_clock)
{
    update_LCD_control();

    if (cpu_clock < prev_cpu_clock) {
        clock += ((1 << 16) - prev_cpu_clock) + cpu_clock;
    }
    else {
        clock += (cpu_clock - prev_cpu_clock);
    }
    prev_cpu_clock = cpu_clock;

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
            draw_scanline();
            change_mode(HBLANK);
        }
        break;

    case HBLANK:
        if (clock >= 204) {
            clock -= 204;
            increment_line();
            
            if (line == 143) {
                window->draw_frame();
                memory->set_interrupt(interrupt::VBLANK_bit);
                change_mode(VBLANK);
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
    update_STAT_register();
}

void GPU::update_STAT_register()
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
    STAT_reg = (STAT_reg & ~3) | (int)mode;
}

void GPU::increment_line()
{
    line++;
    memory->write(reg::LY, line);
    bool coincidence_flag = memory->read(reg::LYC) == memory->read(reg::LY);
    STAT_reg = utils::set_cond(STAT_reg, 2, coincidence_flag);
}

void GPU::build_framebuffer()
{
    render_background();
    render_sprites();
}

void GPU::set_bg_palette()
{
    u8 bgp = memory->read(reg::BGP);
    color_palette[0] = COLORS[bgp & 3];
    color_palette[1] = COLORS[(bgp >> 2) & 3];
    color_palette[2] = COLORS[(bgp >> 4) & 3];
    color_palette[3] = COLORS[(bgp >> 6) & 3];
}

void draw_scanline()
{
    
}

void GPU::read_tile(std::vector<u8>::iterator dest, std::vector<u8>::iterator src)
{
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            // two bytes per line, contain lsb and msb of color
            int byte_ind = 2 * (7 - j);
            u8 lsb = (src[byte_ind] >> (7 - i)) & 1;
            u8 msb = (src[byte_ind + 1] >> (7 - i)) & 1;
            int color = ((msb << 1) | lsb) & 3;
            dest[256 * j + i] = color_palette[color];
        }
    }
}

void GPU::read_sprite_tile(std::vector<u8>::iterator dest, std::vector<u8>::iterator src,
    bool flip_x, bool flip_y)
{
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            // two bytes per line, contain lsb and msb of color
            int byte_ind = 2 * (flip_y ? i : 7 - j);
            u8 lsb = (src[byte_ind] >> (flip_x ? i : 7 - i)) & 1;
            u8 msb = (src[byte_ind + 1] >> (flip_x ? i: 7 - i)) & 1;
            int color = ((msb << 1) | lsb) & 3;
            if (color != 0) {
                dest[2 * (176 * j + i)] = color_palette[color];
                dest[2 * (176 * j + i) + 1] = 1;
            }
        }
    }
}

void GPU::render_background()
{
    if (!LCD_control.enable_display) 
        return;

    u16 tile_map = LCD_control.bg_tile_map ? 0x9c00 : 0x9800;
    u16 tile_data = LCD_control.tile_data_table ? 0x8000 : 0x9000;
    bool signed_map = !LCD_control.tile_data_table;

    for (int tile_i = 0; tile_i < 32; tile_i++) {
        for (int tile_j = 0; tile_j < 32; tile_j++) {
            // locate the tiles in memory
            int map_index = 32 * tile_i + tile_j;

            // read the tile map and determine address of tile
            u16 tile_addr;
            if (signed_map) {
                int tile_num = (i8)memory->read(tile_map + map_index);
                tile_addr = tile_data + (16 * tile_num);
            }
            else {
                int tile_num = memory->read(tile_map + map_index);
                tile_addr = tile_data + (16 * tile_num);
            }
            // relative to start of VRAM
            tile_addr -= 0x8000;

            int pixel_y = 256 - 8 * (tile_i + 1);
            int pixel_x = 8 * tile_j;
            int pixel_index = 256 * pixel_y + pixel_x;
            
            read_tile(framebuffer.begin() + pixel_index, memory->video_RAM.begin() + tile_addr);
        }
    }
}

void GPU::render_sprites()
{
    if (!LCD_control.enable_sprites) {
        return;
    }

    std::vector<u8>::iterator sprite_data = memory->sprite_attribute_table.begin();
    sprite_texture.assign(sprite_texture.size(), 0);

    for (int i = 39; i >= 0; i--) {
        int byte_ind = 4 * i;
        int ypos = sprite_data[byte_ind];
        int xpos = sprite_data[byte_ind + 1];

        if (xpos == 0 || xpos >= 168 || ypos == 0 || ypos >= 160) { // sprite hidden
            continue;
        }

        int tile_num = sprite_data[byte_ind + 2];
        int flags = sprite_data[byte_ind + 3];

        bool behind_bg = utils::bit(flags, 7);
        bool flip_y = utils::bit(flags, 6);
        bool flip_x = utils::bit(flags, 5);
        bool palette_num = utils::bit(flags, 4);

        u8 obp = palette_num ? memory->read(reg::OBP1) : memory->read(reg::OBP0);

        color_palette[0] = COLORS[obp & 3];
        color_palette[1] = COLORS[(obp >> 2) & 3];
        color_palette[2] = COLORS[(obp >> 4) & 3];
        color_palette[3] = COLORS[(obp >> 6) & 3];

        // position of lower left corner in framebuffer
        int pixel_index = 176 * (160 - ypos) + xpos;
        // always taken from first tile data table
        u16 tile_addr = 16 * tile_num;
        
        if (LCD_control.double_sprite_height) {
            u8 upper = tile_num & 0xfe;
            read_sprite_tile(sprite_texture.begin() + 2 * pixel_index, 
                memory->video_RAM.begin() + 16*upper, flip_x, flip_y);
            u8 lower = tile_num | 1;                
            pixel_index = 176 * (160 - ypos - 8) + xpos;
            read_sprite_tile(sprite_texture.begin() + 2 * pixel_index, 
                memory->video_RAM.begin() + 16*lower, flip_x, flip_y);
        }
        else {
            read_sprite_tile(sprite_texture.begin() + 2 * pixel_index, 
                memory->video_RAM.begin() + 16*tile_num, flip_x, flip_y);
        }
    }
}

void GPU::render_window()
{

}

void GPU::update_LCD_control()
{
    u8 byte = memory->read(reg::LCDC);
    LCD_control.enable_display = (byte >> 7) & 1;
    LCD_control.win_tile_map = (byte >> 6) & 1;
    LCD_control.win_enable = (byte >> 5) & 1;
    LCD_control.tile_data_table = (byte >> 4) & 1;
    LCD_control.bg_tile_map = (byte >> 3) & 1;
    LCD_control.double_sprite_height = (byte >> 2) & 1;
    LCD_control.enable_sprites = (byte >> 1) & 1;
    LCD_control.bg_priority = byte & 1;
}