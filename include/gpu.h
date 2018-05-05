#ifndef GPU_H
#define GPU_H

#include "definitions.h"

class GPU 
{
public:
    GPU(u8 *mem = nullptr);
    ~GPU();
    void set_memory(u8 *mem);
    float* construct_framebuffer();

private:
    float* framebuffer;
    u8 *memory;

    void render_background();
    void render_window();
    void render_sprites();

    const u16 lcd_control = 0xff40;
    const u16 scroll_x = 0xff40; 
    const u16 scroll_y = 0xff41;
    const u16 win_x = 0xff4b;
    const u16 win_y = 0xff4a;

    const u8 lcd_enable = 1 << 7;
    const u8 win_tile_map_select = 1 << 6;
    const u8 win_enable = 1 << 5;
    const u8 tile_data_select = 1 << 4;
    const u8 bg_tile_map_select = 1 << 3;
    const u8 sprite_size_select = 1 << 2;
    const u8 sprite_enable = 1 << 1;
    const u8 bg_win_enable = 1;

    const float color[4] = { 0x00 / 255, 0x66 / 255, 0xb2 / 255, 0xff / 255 };
};

#endif