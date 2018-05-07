#include "gpu.h"
#include <iostream>

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
void GPU::read_tile(float *dest, u16 tile_addr)
{
    // tiles are stored in 16 bytes
    for (u8 i = 0; i < 16; i++) {
        u8 byte = memory[tile_addr + i];
        for (u8 j = 0; j < 4; j++) {
            // 3 is a mask for first two bits
            int color = (byte >> (2 * j)) & 3;
            // tiles are not contiguous in final framebuffer since they span multiple lines
            int pixel_ind;
            if (INVERT_TILES) {
                pixel_ind = (constants::screen_w * (7 - i/2)) + (j + ((i % 2) * 4));
            }
            else {
                pixel_ind = (constants::screen_w * (i/2)) + (j + ((i % 2) * 4));
            }
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
                    i16 tile_num = memory[tile_map + map_index];
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
                read_tile(framebuffer + framebuf_index, tile_addr);
            }
        }
    }
    else {
    }
}