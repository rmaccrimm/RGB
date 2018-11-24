#ifndef GPU_H
#define GPU_H

#include <vector>
#include "definitions.h"
#include "mmu.h"
#include "window.h"

class GPU 
{
public:
    enum Mode { HBLANK, VBLANK, OAM, VRAM };

    struct {
        bool enable_display;
        u16 win_tile_map_addr;
        bool win_enable;
        u16 tile_data_addr;
        u16 bg_tile_map_addr;
        bool double_sprite_height;
        bool enable_sprites;
        bool bg_priority;
        bool signed_tile_map;
    } LCD_control;

    GPU(Memory *mem, GameWindow *win);
    void build_framebuffer();
    void step(unsigned int cpu_clock);

private:
    Memory *memory;
    GameWindow *window;
    std::vector<u8> framebuffer;
    std::vector<u8> sprite_texture;

    int clock;
    int line;
    Mode mode;

    bool stat_irq_signal; // Used to trigger LCDSTAT interrupt

    u8 &STAT_reg;

    int prev_cpu_clock;
    
    void draw_scanline();
    void read_tile(std::vector<u8>::iterator dest, std::vector<u8>::iterator src);
    void read_sprite_tile(std::vector<u8>::iterator dest, std::vector<u8>::iterator src,
        bool flip_x, bool flip_y);
    void render_background();
    void render_window();
    void render_sprites();
    void change_mode(Mode m);
    void increment_line();
    void set_bg_palette();
    void update_STAT_register();
    void update_LCD_control();

    u8 color_palette[4];

    const u8 COLORS[4] = { 
        0xff,   // 00 white
        0xb2,   // 01 dark gray
        0x66,   // 10 light gray
        0x00    // 11 black
    };

    static const int LCD_WIDTH;
    static const int LCD_HEIGHT;
    static const u16 TILE_MAP_0_ADDR;
    static const u16 TILE_MAP_1_ADDR;
    static const u16 TILE_DATA_0_ADDR;
    static const u16 TILE_DATA_1_ADDR;
    static const u16 VRAM_ADDR;
    static const u16 OAM_ADDR;
};

#endif