#include "gpu.h"
#include "registers.h"
#include "interrupts.h"
#include "debug.h"
#include <iostream>
#include <iomanip>
#include <cassert>
#include <algorithm>

const int GPU::LCD_WIDTH = 160;
const int GPU::LCD_HEIGHT = 144;
const int GPU::BACKGROUND_DIM = 256; // background is 256x256 pixels
const int GPU::TILE_MAP_DIM = 32; // tile map is 32x32
const int GPU::TILE_DIM = 8; // tile are 8x8 pixels
const int GPU::BYTES_PER_TILE = 16;
const u16 GPU::TILE_MAP_0_ADDR = 0x9800;
const u16 GPU::TILE_MAP_1_ADDR = 0x9c00;
const u16 GPU::TILE_DATA_0_ADDR = 0x9000;
const u16 GPU::TILE_DATA_1_ADDR = 0x8000;
const u16 GPU::VRAM_ADDR = 0x8000;
const u16 GPU::OAM_ADDR = 0xfe00;

GPU::GPU(Interrupts *inter, GameWindow *win): 
    interrupts{inter},
    window(win), 
    clock(0), 
    line(0),
    mode(OAM), 
    frame_drawn(false)
{
    video_RAM.resize(0x2000, 0); // 8kB
    sprite_attribute_table.resize(0xa0, 0);
    screen_texture.resize(LCD_WIDTH * LCD_HEIGHT);
    transparent.resize(LCD_HEIGHT);
    for (auto &row: transparent) {
        row.resize(LCD_WIDTH, 0);
    }
    for (int i = 0xff40; i <= 0xff4b; i++) {
        registers[i] = 0;
    }
}

void GPU::step(unsigned int cycles)
{
    clock += cycles;

    switch (mode)
    {
    case OAM:
    // First step in drawing scanline, OAM being scanned and not accessible by CPU
        if (clock >= 80) {
            clock -= 80;
            change_mode(VRAM);
        }
        break;

    case VRAM:
    // Second step of drawing a scanline, VRAM and OAM not accessible by CPU
        if (clock >= 172) {
            clock -= 172;
            // At end of scanline, draw and switch to horizontal blank mode
            draw_scanline();
            change_mode(HBLANK);
        }
        break;

    case HBLANK:
        if (clock >= 204) {
            clock -= 204;
            increment_line();
            
            if (line == 143) {
                // On last line, update the screen and switch to vertical blank mode 
                window->draw_frame(screen_texture.data());
                interrupts->set(Interrupts::VBLANK_bit);
                change_mode(VBLANK);
                frame_drawn = true;
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
                // Clear bit 0 of interrupt request
                interrupts->clear(Interrupts::VBLANK_bit);
                change_mode(OAM);
            }
        }
        break;    
    }
    update_STAT_register();
}

u8 GPU::read(u16 addr) 
{
    if (addr >= 0x8000 && addr <= 0x9fff) {
        // VRAM
        if (mode == VRAM && LCD_control.enable_display) {
            // GPU is accessing VRAM during this period, so inaccessible by CPU during mode 3
            return 0xff;
        }
        else {
            return video_RAM[addr - 0x8000];
        }
    }
    else if (addr >= 0xfe00 && addr <= 0xfe9f) {
        // OAM
        if ((mode == VRAM || mode == OAM) && LCD_control.enable_display) {
            // OAM inaccessible during both mode 2 and 3
            return 0xff;
        }
        else {
            return sprite_attribute_table[addr - 0xfe00];
        }
    }
    else if (addr >= 0xff40 && addr <= 0xff4b) {
        // Control registers - DMA access handled by MMU, not GPU
        assert(addr != reg::DMA);
        if (addr == reg::STAT) {
            // First bit always set 
            return registers[addr] |= 0x80;
        }
        return registers[addr];
    }
    else {
        assert(false);
        return 0;
    }
}

void GPU::write(u16 addr, u8 data)
{
    if (addr >= 0x8000 && addr <= 0x9fff) {
        // VRAM
        if (mode == VRAM && LCD_control.enable_display) {
            // GPU is accessing VRAM during this period, so inaccessible by CPU during mode 3
            return;
        }
        video_RAM[addr - 0x8000] = data;
    }
    else if (addr >= 0xfe00 && addr <= 0xfe9f) {
        // OAM
        if ((mode == VRAM || mode == OAM) && LCD_control.enable_display) {
            // OAM inaccessible during both mode 2 and 3
            return;
        }
        sprite_attribute_table[addr - 0xfe00] = data;
    }
    else if (addr >= 0xff40 && addr <= 0xff4b) {
        // Control registers
        switch (addr) 
        {
        case reg::DMA:
            // handled by MMU
            assert(false);
        case reg::STAT: {
            // Lowest 3 bits are read-only
            u8 mask = 0x7;
            data = (data & (~mask)) | (registers[addr] & mask);
            break;
        }
        case reg::LCDC:
            update_LCD_control(data);
            break;
        case reg::BGP:
            for (int i = 0; i < 4; i++) {
                bg_palette[i] = (data >> (2*i)) & 3;
            }
            break;
        case reg::OBP0:
            for (int i = 0; i < 4; i++) {
                sprite_palette[0][i] = (data >> (2*i)) & 3;
            }   
            break;
        case reg::OBP1:
            for (int i = 0; i < 4; i++) {
                sprite_palette[1][i] = (data >> (2*i)) & 3;
            }
            break;
        }   
        registers[addr] = data;
    }
    else {
        assert(false);
    }
}

void GPU::update_STAT_register()
{
    
    bool prev_sig = stat_irq_signal;
    u8 stat = registers[reg::STAT];

    // Masks for STAT register - coincidence flag set when current line = LYC register 
    u8 coincidence_enable = 1 << 6;
    u8 oam_enable = 1 << 5;
    u8 vblank_enable = 1 << 4;
    u8 hblank_enable = 1 << 3;
    u8 coincidence_set = 1 << 2;

    // Internal signal which can trigger STAT interrupts. Depends on a number of factors
    stat_irq_signal = ((stat & coincidence_enable) && (stat & coincidence_set)) ||
                      ((stat & hblank_enable) && mode == HBLANK) ||
                      ((stat & oam_enable) && mode == OAM) ||
                      ((stat & (vblank_enable | oam_enable)) && mode == VBLANK);
    // set LCDSTAT interrupt request if internal signal goes from 0 to 1. 
    if (!prev_sig && stat_irq_signal) {
        interrupts->set(Interrupts::LCDSTAT_bit);
    }
}

void GPU::change_mode(Mode m)
{
    mode = m;
    // TODO - If LCD is off, set to 0
    registers[reg::STAT] = (registers[reg::STAT] & ~3) | (int)mode;
}

void GPU::increment_line()
{
    line++;
    registers[reg::LY] = line;
    // Set coincidence flag in STAT whenever current line = LYC
    bool coincidence_flag = registers[reg::LYC] == registers[reg::LY];
    registers[reg::STAT] = utils::set_cond(registers[reg::STAT], 2, coincidence_flag);
}

void GPU::update_color_palettes()
{
    u8 bgp = registers[reg::BGP];
    u8 obp1 = registers[reg::OBP1];
    u8 obp0 = registers[reg::OBP0];
    for (int i = 0; i < 4; i++) {
        // Color palettes are stored in a single byte, 2 bits per color
        bg_palette[i] = (bgp >> (2*i)) & 3;
        sprite_palette[0][i] = (obp0 >> (2*i)) & 3;
        sprite_palette[1][i] = (obp1 >> (2*i)) & 3;
    }
}

u8 GPU::read_pixel(std::vector<u8>::iterator &tile_data, int x, int y, bool invert_y, bool invert_x)
{
    // 2 bit data for each pixel spread across 2 bytes, one storing the lower bit and one the upper
    int byte_ind = 2 * (invert_y ? TILE_DIM - 1 - y : y);
    u8 lsb = (tile_data[byte_ind] >> (invert_x ? x : TILE_DIM - 1 - x)) & 1;
    u8 msb = (tile_data[byte_ind + 1] >> (invert_x ? x: TILE_DIM - 1 - x)) & 1;
    return ((msb << 1) | lsb) & 3;
}

void GPU::draw_scanline()
{
    if (LCD_control.enable_display) {
        draw_background();
        draw_window();
        draw_sprites();
    }
    else {
        // Blank screen
        for (int i = 0; i < LCD_WIDTH; i++) {
            draw_pixel(i, line, 0);
        }
    }
}

void GPU::draw_pixel(int x, int y, int color)
{
    // OpenGL texture coordinates are bottom-up whereas GB is top-down
    screen_texture[(LCD_WIDTH * (LCD_HEIGHT - 1 - y)) + x] = color;
}

u8& GPU::get_pixel(int x, int y)
{
    // OpenGL texture coordinates are bottom-up whereas GB is top-down
    return screen_texture[(LCD_WIDTH * (LCD_HEIGHT - 1 - y)) + x];
}

void GPU::draw_background()
{
    // Coordinates of upper left corner of screen on 256 x 256 background
    int x = registers[reg::SCROLLX];
    int y = registers[reg::SCROLLY];
    auto tile_data_base = video_RAM.begin() + (LCD_control.tile_data_addr - VRAM_ADDR);

    // Screen-space coordinates are (i, line) 
    for (int i = 0; i < LCD_WIDTH; i++) {
        // Pixel coordinates on the 256 x 256 background
        int bg_x = (x + i) % BACKGROUND_DIM;
        int bg_y = (y + line) % BACKGROUND_DIM;

        // Coordinates of the tile containing current pixel on the 32 x 32 tile map
        int tile_map_x = bg_x / TILE_DIM;
        int tile_map_y = bg_y / TILE_DIM;
        int tile_map_index = (TILE_MAP_DIM * tile_map_y) + tile_map_x; 

        // Lookup the index for the tile in the tile map
        int tile_index = video_RAM[LCD_control.bg_tile_map_addr + tile_map_index - VRAM_ADDR];
        if (LCD_control.signed_tile_map) {
            tile_index = (i8)tile_index;
        }
        // Pixel coordinates within the 8 x 8 tile
        int tile_x = bg_x % TILE_DIM;
        int tile_y = bg_y % TILE_DIM;

        auto tile_ptr = tile_data_base + (BYTES_PER_TILE * tile_index);
        // Background tiles are never inverted on x or y 
        int color = read_pixel(tile_ptr, tile_x, tile_y, false, false);
        // The 2-bit pixel data read is the index for the color palette
        draw_pixel(i, line, bg_palette[color]);
        transparent[line][i] = color == 0;
    }
}

void GPU::draw_sprites()
{
    if (!LCD_control.enable_sprites) {
        return;
    }

    std::vector<u8>::iterator sprite_data = sprite_attribute_table.begin();
    int sprite_size = (LCD_control.double_sprite_height ? 2 : 1) * TILE_DIM;

    std::vector<std::pair<int, int>> sprites;

    // Construct a list of the sprites on the current scan line
    for (int i = 0; i < 40; i++) {
        // The index of the first byte for the sprite in the sprite attribute table
        int byte_ind = 4 * i;
        int y_pos = sprite_data[byte_ind] - 16;
        int x_pos = sprite_data[byte_ind + 1] - 8;

        // Sprites can be disabled by placing them offscreen (x: -8 to 0, 1)
        bool offscreen = x_pos == -8 || x_pos >= 160;
        bool on_current_scanline = (line >= y_pos) && (line < y_pos + sprite_size);

        if (offscreen || !on_current_scanline) {
            continue;
        }
        sprites.emplace_back(x_pos, i);
    }
    // Sprite priority is determined by x coordinate
    sort(sprites.begin(), sprites.end());

    /*  Maximum of 10 sprites are drawn for each scanline. By default sorted by descending x value
        so draw the last 10 in the list
    */
    int s = std::max((int)(sprites.size() - 10), 0);
    for (auto it = sprites.rbegin() + s; it < sprites.rend(); it++) {
        int byte_ind = it->second * 4;

        // 4 bytes per sprite
        int y_pos = sprite_data[byte_ind] - 16;
        int x_pos = sprite_data[byte_ind + 1] - 8;
        u8 tile_num = sprite_data[byte_ind + 2];
        int flags = sprite_data[byte_ind + 3];

        bool behind_bg = utils::bit(flags, 7);
        bool flip_y = utils::bit(flags, 6);
        bool flip_x = utils::bit(flags, 5);
        bool palette_num = utils::bit(flags, 4);

        // Might start or end in the middle of a tile if partially offscreen
        for (int i = std::max(0, x_pos); i < std::min(LCD_WIDTH, x_pos + TILE_DIM); i++) {

            if (behind_bg && !transparent[line][i]) {
                continue;
            }

            int pixel_x = i - x_pos;
            int pixel_y = line - y_pos;
            int color;
            if (LCD_control.double_sprite_height) {
                /*  For double-tile sprites, lower tile is found by ignoring the first bit of the
                    tile number and the lower by setting the first bit
                */
                u8 upper_tile_index = tile_num & 0xfe;
                u8 lower_tile_index = tile_num | 1;

                bool upper = pixel_y < 8;
                if (upper) {
                    // Also have to invert the tile order for double-height sprites
                    tile_num = flip_y ? lower_tile_index : upper_tile_index;
                }
                else {
                    tile_num = flip_y ? upper_tile_index : lower_tile_index;
                    pixel_y -= 8;
                }
                auto tile_addr = video_RAM.begin() + (BYTES_PER_TILE * tile_num);
                color = read_pixel(tile_addr, pixel_x, pixel_y, flip_y, flip_x);
            }
            else {
                auto tile_addr = video_RAM.begin() + (BYTES_PER_TILE * tile_num);
                color = read_pixel(tile_addr, pixel_x, pixel_y, flip_y, flip_x);
            }
            if (color == 0) {
                continue;
            }
            draw_pixel(i, line, sprite_palette[palette_num][color]);
        }        
    }
}

void GPU::draw_window()
{
    if (!LCD_control.enable_window) {
        return;
    }
    // Screen-space coordinates of the window's upper left corner
    int x = registers[reg::WX] - 7;
    int y = registers[reg::WY];
    if (y > line) {
        // Window not visible on current scanline
        return;
    }

    auto tile_data = video_RAM.begin() + (LCD_control.tile_data_addr - VRAM_ADDR);
    // (i, line) are the screen space pixel coordinates
    for (int i = std::max(x, 0); i < LCD_WIDTH; i++) {
        // Window-space coordinates of the current pixel, translated left/up as window moves
        int window_x = i - x;
        int window_y = line - y;

        // Coordinates of the tile containing current pixel in 32 x 32 tile map
        int tile_map_x = (i - x) / TILE_DIM;
        int tile_map_y = (line - y) / TILE_DIM;
        int tile_map_index = (TILE_MAP_DIM * tile_map_y) + tile_map_x; 

        int tile_index = video_RAM[LCD_control.win_tile_map_addr + tile_map_index - VRAM_ADDR];        
        if (LCD_control.signed_tile_map) {
            tile_index = (i8)tile_index;
        }
        // Coordinates of the pixel within the 8 x 8 tile
        int tile_x = window_x % TILE_DIM;
        int tile_y = window_y % TILE_DIM;

        auto tile_ptr = tile_data + (BYTES_PER_TILE * tile_index);
        int color = read_pixel(tile_data, tile_x, tile_y, false, false);
        draw_pixel(i, line, bg_palette[color]);
        transparent[line][i] = color == 0;
    }
}

/*  From Pan Docs:
        Bit 7 - LCD Display Enable             (0=Off, 1=On)
        Bit 6 - Window Tile Map Display Select (0=9800-9BFF, 1=9C00-9FFF)
        Bit 5 - Window Display Enable          (0=Off, 1=On)
        Bit 4 - BG & Window Tile Data Select   (0=8800-97FF, 1=8000-8FFF)
        Bit 3 - BG Tile Map Display Select     (0=9800-9BFF, 1=9C00-9FFF)
        Bit 2 - OBJ (Sprite) Size              (0=8x8, 1=8x16)
        Bit 1 - OBJ (Sprite) Display Enable    (0=Off, 1=On)
        Bit 0 - BG/Window Display/Priority     (0=Off, 1=On) 
*/
void GPU::update_LCD_control(u8 byte)
{
    LCD_control.enable_display = (byte >> 7) & 1;
    LCD_control.win_tile_map_addr = (byte >> 6) & 1 ? TILE_MAP_1_ADDR : TILE_MAP_0_ADDR;
    LCD_control.enable_window = (byte >> 5) & 1;
    LCD_control.tile_data_addr = (byte >> 4) & 1 ? TILE_DATA_1_ADDR : TILE_DATA_0_ADDR;
    LCD_control.signed_tile_map = !((byte >> 4) & 1);
    LCD_control.bg_tile_map_addr = (byte >> 3) & 1 ? TILE_MAP_1_ADDR : TILE_MAP_0_ADDR;
    LCD_control.double_sprite_height = (byte >> 2) & 1;
    LCD_control.enable_sprites = (byte >> 1) & 1;
    LCD_control.bg_priority = byte & 1;    
}

void GPU::dma_transfer(std::vector<u8>::iterator src) 
{
    // 40 tiles - each tiles has 4 bytes
    std::copy(src, src + 0xa0, sprite_attribute_table.begin());
}