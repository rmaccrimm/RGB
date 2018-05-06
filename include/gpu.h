#ifndef GPU_H
#define GPU_H

#include "definitions.h"

class GPU 
{
public:
    GPU(u8 *mem = nullptr);
    ~GPU();
    void set_memory(u8 *mem);
    float* build_framebuffer();

    static const u16 LCDC = 0xff40;
    static const u16 SCROLLX = 0xff43; 
    static const u16 SCROLLY = 0xff42;
    static const u16 WX = 0xff4b;
    static const u16 WY = 0xff4a;

    static const u16 TILE_MAP_0 = 0x9800;
    static const u16 TILE_MAP_1 = 0x9c00;
    static const u16 TILE_DATA_0 = 0x9000; // signed tile index
    static const u16 TILE_DATA_1 = 0x8000; // unsigned tile index

private:
    float* framebuffer;
    u8 *memory;

    void read_tile(float *pixels, u16 tile_addr);
    void render_background();
    void render_window();
    void render_sprites();

    const u8 LCD_ENABLE = 1 << 7;
    const u8 WIN_TILE_MAP_SELECT = 1 << 6;
    const u8 WIN_ENABLE = 1 << 5;
    const u8 TILE_DATA_SELECT = 1 << 4;
    const u8 BG_TILE_MAP_SELECT = 1 << 3;
    const u8 SPRITE_SIZE_SELECT = 1 << 2;
    const u8 SPRITE_ENABLE = 1 << 1;
    const u8 BG_ENABLE = 1;    

    const float COLORS[4] = { 
        (float)0x00 / 255,
        (float)0x66 / 255, 
        (float)0xb2 / 255, 
        (float)0xff / 255 
    };
};

#endif