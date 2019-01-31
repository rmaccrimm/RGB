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
    /*  The interrupts object is shared with the cpu and memory, GPU triggers the window to 
        draw once per frame, once full frame is drawn and vertical blank mode is entered
    */
    GPU(Interrupts *inter, GameWindow *win);

    // Advance GPU by the given number of CPU cycles 
    void step(unsigned int cpu_cycles);

    // Access VRAM, OAM and GPU registers
    u8 read(u16 addr);
    void write(u16 addr, u8 data);

    // Initiate DMA transfer. Copies 160 bytes from src to OAM (sprite table)
    void dma_transfer(std::vector<u8>::iterator src);

    // 160 x 144
    static const int LCD_WIDTH;
    static const int LCD_HEIGHT;

    // Flag used controlling timing between frames, must be reset externally
    bool frame_drawn;
    
private:
    /*  Four modes the GPU cycles through. Each scanline starts in mode 2, in which OAM is being
        accessed by the GPU and inaccessible by the CPU. In mode 3 VRAM becomes inaccessible to the
        CPU and OAM is still inaccessible. OAM is accessible at any point, however, by direct 
        memory acces (DMA) transfer. At the end of each scanline, horizontal blank mode (0) is
        entered. After the last scanline, vertical blank mode (1) is entered, which lasts for 10
        additional scanlines.

        The exact timing between OAM and VRAM modes varies per scanline depending on a number of 
        factors, but is treated as a constant here.
    */
    enum Mode { HBLANK, VBLANK, OAM, VRAM };

    // fields described by 8 bits in LCDC register
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

    int clock;
    int line;
    Mode mode;

    // Used to trigger LCDSTAT interrupt
    bool stat_irq_signal; 

    GameWindow *window;
    Interrupts *interrupts;

    // Data passed to window to be drawn to texture 
    std::vector<u8> screen_texture;

    // VRAM, addresses 0x8000 - 0x9fff (8kB)
    std::vector<u8> video_RAM;
    // object attribute memory (OAM), addresses 0xfe00 - 0xfe90 (160 bytes = 40 sprites)
    std::vector<u8> sprite_attribute_table;

    // Video control registers - addresses 0xff40 - 0xff4b
    std::map<u16, u8> registers;

    // Color palettes
    u8 bg_palette[4];
    u8 sprite_palette[2][4];
    
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

    // Retrieve pixel data from a tile stored in VRAM 
    u8 read_pixel(std::vector<u8>::iterator &tile_data, int x, int y, bool invert_y, bool invert_x);

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