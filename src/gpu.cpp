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
    assert(x_low >= 0 && x_high < constants::tile_size);
    assert(y_low >= 0 && y_high < constants::tile_size);

    for (int i = x_low; i <= x_high; i++) {
        for (int j = y_low; j <= y_high; j++) {
            // 4 pixels per byte
            u8 byte = memory[tile_addr + (2 * j) + (i / 4)];
            // 3 is a mask for first two bits
            int color = (byte >> (2 * (i % 4)) & 3);
            // tiles are not contiguous in final framebuffer since they span multiple lines
            int pixel_ind;
            if (INVERT_TILES) {
                pixel_ind = constants::screen_w * (7 - j) + i;
            }
            else {
                pixel_ind = constants::screen_w * j + i;
            }
            // same rgb values for gray scale
            for (u8 k = 0; k < 3; k++) {
                dest[3 * (pixel_ind) + k] = COLORS[color];
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
        // tiles are 8x8, full background is 256x256
        u8 bg_h = constants::screen_h / 8;
        u8 bg_w = constants::screen_w / 8; 
        u8 scroll_x = memory[SCROLLX];
        u8 scroll_y = memory[SCROLLY];

        for (u8 i = 0; i < bg_h; i++) {
            for (u8 j = 0; j < bg_w; j++) {
                u16 tile_addr;
                u8 tile_index = (32 * ((scroll_y + i) % 32)) + ((scroll_x + j) % 32);
                u16 map_index = (32 * i) + j;
                if (signed_map) {
                    i8 tile_num = (i8)memory[tile_map + map_index];
                    tile_addr = tile_data + (16 * tile_num);
                }
                else {
                    u16 tile_num = memory[tile_map + map_index];
                    tile_addr = tile_data + (16 * tile_num);
                }
                // index of lower left pixel for tile
                int pixel_index;
                if (INVERT_MAP) {
                    pixel_index = constants::screen_w * (8 * (bg_h - 1 - i)) + (8 * j);
                }
                else {
                    pixel_index = constants::screen_w * (8 * i) + (8 * j);
                }
                // the framebuffer stores rgb values for each pixel
                int framebuf_index = 3 * pixel_index;
                read_tile(framebuffer + framebuf_index, tile_addr, 0, 0, 7, 7);
            }
        }
    }
    else {
    }
}