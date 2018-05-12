#include "gpu.h"
#include <iostream>
#include <cassert>

GPU::GPU(u8 *mem): memory(mem) 
{
    framebuffer = new float[3 * constants::screen_h * constants::screen_w];
}

GPU::~GPU()
{
    delete framebuffer;
}

void GPU::set_memory(u8 *mem) { memory = mem; }

float* GPU::build_framebuffer()
{
    render_background();
    return framebuffer;
}

// dest is a pointer to the first pixel in the framebuffer where the tile will be loaded
void GPU::read_tile(float *dest, u16 tile_addr, u8 x_low, u8 y_low, u8 x_high, u8 y_high)
{
    assert(dest);
    assert(x_low >= 0 && x_high < constants::tile_size);
    assert(y_low >= 0 && y_high < constants::tile_size);

    for (int i = x_low; i <= x_high; i++) {
        for (int j = y_low; j <= y_high; j++) {
            // 4 pixels per byte
            int byte_ind;
            if (INVERT_TILES) {
                byte_ind = (2 * (7 - j) + (i/4));
            }
            else {
                byte_ind = (2 * j) + (i / 4);
            }
            u8 byte = memory[tile_addr + byte_ind];
            // 3 is a mask for first two bits
            int color = (byte >> (2 * (i % 4)) & 3);
            assert(color >= 0 && color <= 3);
            // tiles are not contiguous in final framebuffer since they span multiple lines
            int pixel_ind = constants::screen_w * j + i;
            // same rgb values for gray scale
            for (int k = 0; k < 3; k++) {
                dest[3 * pixel_ind + k] = COLORS[color];
            }
        }
    }
}

void GPU::render_background()
{
    u8 lcd_control = memory[LCDC];
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

        int scroll_x = memory[SCROLLX];
        int scroll_y = memory[SCROLLY];

        int bg_h = constants::screen_h / 8;
        int bg_w = constants::screen_w / 8; 

        int y_start_tile = scroll_y / 8;
        int y_end_tile = (scroll_y + constants::screen_h) / 8;
        int x_start_tile = scroll_x / 8;
        int x_end_tile = (scroll_y + constants::screen_w) / 8;

        int pi = 0;
        for (int i = y_start_tile; i < y_end_tile; i++) {
            int pj = 0;
            for (int j = x_start_tile; j < x_end_tile; j++) {
                int map_index = 32 * (i % 32) + (j % 32);
                assert(map_index < 32 * 32);
                u16 tile_addr;
                if (signed_map) {
                    i8 tile_num = (i8)memory[tile_map + map_index];
                    assert(tile_num < 128 && tile_num >= -128);
                    tile_addr = tile_data + (16 * tile_num);
                }
                else {
                    u8 tile_num = memory[tile_map + map_index];
                    assert(tile_num >= 0 && tile_num < 256);
                    tile_addr = tile_data + (16 * tile_num);
                }
                // index of lower left pixel for tile
                int pixel_index;
                if (INVERT_MAP) {
                    pixel_index = constants::screen_w * (8 * (bg_h - 1 - pi) + scroll_y) + (8 * pj);
                }
                else {
                    pixel_index = constants::screen_w * (8 * pi) + (8 * pj);
                }
                // the framebuffer stores rgb values for each pixel
                assert(pixel_index >= 0 && pixel_index < constants::screen_h * constants::screen_w);
                int framebuf_index = 3 * pixel_index;

                int xl = 0;
                int yl = 0;
                int xh = 7;
                int yh = 7;
                if (i == y_start_tile) {
                    yh = 7 - (scroll_y % 8);
                }
                if (i == y_end_tile - 1 && i > 0) {
                    yl = (scroll_y + constants::screen_h) % 8;
                }
                if (j == x_start_tile) {
                    xl = scroll_x % 8;
                }
                if (j == x_end_tile - 1 && j > 0) {
                    xh = 7 - (scroll_x + constants::screen_w) % 8;
                }
                read_tile(&framebuffer[framebuf_index], tile_addr, xl, yl, xh, yh);
                pj++;
            }
            pi++;
        }
    }
    else {
    }
}