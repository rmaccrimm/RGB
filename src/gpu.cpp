#include "gpu.h"
#include "registers.h"
#include "interrupts.h"
#include "debug.h"
#include <iostream>
#include <iomanip>
#include <cassert>
#include <algorithm>

GPU::GPU(Memory *mem, GameWindow *win): memory(mem), window(win), clock(0), line(0), mode(OAM)
{
    framebuffer = new u8[256 * 256];
}

GPU::~GPU()
{
    delete framebuffer;
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
                window->draw_frame(framebuffer, memory->read(reg::SCROLLX), memory->read(reg::SCROLLY));
                // Set bit 0 of interrupt request
                u8 int_request = memory->read(reg::IF);
                memory->write(reg::IF, utils::set(int_request, interrupt::VBLANK));
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
                memory->write(reg::IF, utils::reset(int_request, interrupt::VBLANK));
            }
        }
        break;    
    }
}

void GPU::change_mode(Mode m)
{
    mode = m;
    // Lowest two bits of STAT register contain mode
    u8 stat = memory->read(reg::STAT);
    memory->write(reg::STAT, (stat & ~3) | (int)mode);
}

void GPU::increment_line()
{
    line++;
    memory->write(reg::LY, line);
}

u8* GPU::build_framebuffer()
{
    render_background();
    return framebuffer;
}

void GPU::set_bg_palette()
{
    u8 bgp = memory->read(reg::BGP);
    color_palette[0] = COLORS[bgp & 3];
    color_palette[1] = COLORS[(bgp >> 2) & 3];
    color_palette[2] = COLORS[(bgp >> 4) & 3];
    color_palette[3] = COLORS[(bgp >> 6) & 3];
    window->set_bg_palette(color_palette);
}

// dest is a pointer to the first pixel in the framebuffer where the tile will be loaded
void GPU::read_tile(u8 *dest, u16 tile_addr)
{
    assert(dest);
    u8 *pix_data = memory->get_mem_ptr(tile_addr);

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            // two bytes per line, contain lsb and msb of color
            int byte_ind = 2 * (7 - j);
            u8 lsb = (pix_data[byte_ind] >> (7 - i)) & 1;
            u8 msb = (pix_data[byte_ind + 1] >> (7 - i)) & 1;
            int color = ((msb << 1) | lsb) & 3;
            dest[256 * j + i] = color;
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
            tile_map = reg::TILE_MAP_1;
        }
        else {
            tile_map = reg::TILE_MAP_0;
        }
        if (lcd_control & TILE_DATA_SELECT) {
            tile_data = reg::TILE_DATA_1;
            signed_map = false;
        }
        else {
            tile_data = reg::TILE_DATA_0;
            signed_map = true;
        }  

        int tile_i, tile_j;
        int screen_i, screen_j;

        for (tile_i = 0, screen_i = 0; tile_i < 32; tile_i++) {
            for (tile_j = 0, screen_j = 0; tile_j < 32; tile_j++) {

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

                int pixel_y = 256 - 8 * (screen_i + 1);
                int pixel_x = 8 * screen_j;
                int pixel_index = 256 * pixel_y + pixel_x;
                
                read_tile(&framebuffer[pixel_index], tile_addr);
                screen_j++;
            }
            screen_i++;
        }        
    }
}

void GPU::render_window()
{

}

void GPU::render_sprites()
{
    
}

