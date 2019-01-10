#ifndef GPU_H
#define GPU_H

#include <vector>
#include <map>
#include "definitions.h"
#include "window.h"
#include "interrupts.h"

class GPU 
{
public:
    enum Mode { HBLANK, VBLANK, OAM, VRAM };

    struct {
        bool enable_display;
        u16 win_tile_map_addr;
        bool enable_window;
        u16 tile_data_addr;
        u16 bg_tile_map_addr;
        bool double_sprite_height;
        bool enable_sprites;
        bool bg_priority;
        bool signed_tile_map;
    } LCD_control;

    GPU(Interrupts *inter, GameWindow *win);
    void step(unsigned int cpu_clock);

    u8 read(u16 addr);
    void write(u16 addr, u8 data);

    void dma_transfer(std::vector<u8>::iterator src);

    static const int LCD_WIDTH;
    static const int LCD_HEIGHT;

    bool frame_drawn;

private:
    GameWindow *window;
    Interrupts *interrupts;
    std::vector<u8> screen_texture;
    std::vector<u8> video_RAM;
    std::vector<u8> sprite_attribute_table;
    std::map<u16, u8> registers;

    int clock;
    int line;
    Mode mode;
    bool stat_irq_signal; // Used to trigger LCDSTAT interrupt
    // int prev_cpu_clock;
    
    void draw_scanline();
    void draw_pixel(int x, int y, int color);
    void draw_background();
    void draw_sprites();
    void draw_window();
    void change_mode(Mode m);
    void increment_line();
    void update_color_palettes();
    void update_STAT_register();
    void update_LCD_control(u8 byte);
    u8 read_pixel(std::vector<u8>::iterator &tile_data, int x, int y, bool invert_y, bool invert_x);

    u8 bg_palette[4];
    u8 sprite_palette[2][4];

    static const int BACKGROUND_DIM;
    static const int TILE_MAP_DIM;
    static const int TILE_DIM;
    static const int BYTES_PER_TILE;
    static const u16 TILE_MAP_0_ADDR;
    static const u16 TILE_MAP_1_ADDR;
    static const u16 TILE_DATA_0_ADDR;
    static const u16 TILE_DATA_1_ADDR;
    static const u16 VRAM_ADDR;
    static const u16 OAM_ADDR;
};

#endif