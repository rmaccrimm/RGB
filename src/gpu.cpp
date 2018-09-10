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
void GPU::read_tile(u8 *dest, u16 tile_addr, u8 x_low, u8 y_low, u8 x_high, u8 y_high)
{
    assert(dest);
    u8 *pix_data = memory->get_mem_ptr(tile_addr);

    for (int i = x_low; i <= x_high; i++) {
        for (int j = y_low; j <= y_high; j++) {
            // two bytes per line, contain lsb and msb of color
            u16 byte_ind = 2 * (7 - j);
            // u8 l_byte = pix_data[byte_ind];
			u8 l_byte = pix_data[byte_ind];
			u8 h_byte = pix_data[byte_ind + 1];
            u8 lsb = (l_byte >> (7 - i)) & 1;
            u8 msb = (h_byte >> (7 - i)) & 1;
            int color = ((msb << 1) | lsb) & 3;
            // shift pixels to bottom left corner
            int pixel_ind = constants::screen_w * (j - y_low) + (i - x_low);
            // same rgb values for gray scale
            dest[pixel_ind] = color;
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

        int scroll_x = 0; // memory->read(reg::SCROLLX);
        int scroll_y = 0; // memory->read(reg::SCROLLY);

        int bg_h = constants::screen_h / 8;
        int bg_w = constants::screen_w / 8; 

        // determine which tiles visible on screen
        int y_start_tile = 0; //(scroll_y / 8);
        int y_end_tile = 31; //((scroll_y + constants::screen_h - 1) / 8);
        int x_start_tile = 0; //scroll_x / 8;
        int x_end_tile = 31; //(scroll_x + constants::screen_w - 1) / 8;

        int tile_i, tile_j;
        int screen_i, screen_j;

        for (tile_i = y_start_tile, screen_i = 0; tile_i <= y_end_tile; tile_i++) {
            for (tile_j = x_start_tile, screen_j = 0; tile_j <= x_end_tile; tile_j++) {

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


                // Determine where one screen bottom left corner of tile is drawn, this only
                int pixel_y = std::max(
                    (int)constants::screen_h - 8 * (screen_i + 1) + (scroll_y % 8), 0);
                int pixel_x = std::max(8 * screen_j - (scroll_x % 8), 0);
                int pixel_index = constants::screen_w * pixel_y + pixel_x;
                int framebuf_index = pixel_index;
                int xl = 0;
                int yl = 0;
                int xh = 7;
                int yh = 7;

                // first row, upper edge decreases: 7, 6, 5, ...
                if (tile_i == y_start_tile) {
                    yh = 7 - (scroll_y % 8);
                }
                // last row, lower edge: 0, 7, 6, 5, ...
                if (tile_i == y_end_tile) {
                    // handle negatives: a % b -> (a % b + b) % b
                    yl = ((8 - scroll_y) % 8 + 8) % 8;
                }
                // first col, lower edge increases: 0, 1, 2, ...
                if (tile_j == x_start_tile) {
                    xl = scroll_x % 8;
                }
                // last col upper edge: 7, 0, 1, 2, ...
                if (tile_j == x_end_tile) {
                    xh = (7 + scroll_x) % 8;
                }
                read_tile(&framebuffer[framebuf_index], tile_addr, xl, yl, xh, yh);
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

