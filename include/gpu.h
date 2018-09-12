#ifndef GPU_H
#define GPU_H

#include "definitions.h"
#include "mmu.h"
#include "window.h"

class GPU 
{
public:

    enum Mode { HBLANK, VBLANK, OAM, VRAM };

    GPU(Memory *mem, GameWindow *win);
    ~GPU();
    u8* build_framebuffer();
    void step(unsigned int cpu_clock);

private:
    Memory *memory;
    GameWindow *window;
    u8* framebuffer;

    int line;
    int clock;
    Mode mode;
    
    void read_tile(u8 *pixels, u16 tile_addr);
    void render_background();
    void render_window();
    void render_sprites();
    void change_mode(Mode m);
    void increment_line();
    void set_bg_palette();

    u8 color_palette[4];

    const u8 LCD_ENABLE = 1 << 7;
    const u8 WIN_TILE_MAP_SELECT = 1 << 6;
    const u8 WIN_ENABLE = 1 << 5;
    const u8 TILE_DATA_SELECT = 1 << 4;
    const u8 BG_TILE_MAP_SELECT = 1 << 3;
    const u8 SPRITE_SIZE_SELECT = 1 << 2;
    const u8 SPRITE_ENABLE = 1 << 1;
    const u8 BG_ENABLE = 1;    

    const u8 COLORS[4] = { 
        0xff,   // 00 white
        0xb2,   // 01 dark gray
        0x66,   // 10 light gray
        0x00    // 11 black
    };

    // opengl expects framebuffer drawn from bottom up
    const bool INVERT_MAP = true;
    const bool INVERT_TILES = true;
};

#endif