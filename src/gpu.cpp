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

GPU::GPU(Memory *mem, GameWindow *win): 
    memory(mem), 
    window(win), 
    clock(0), 
    line(0),
    mode(OAM), 
    STAT_reg(mem->get_mem_reference(reg::STAT)),
    prev_cpu_clock(0)
{
    screen_texture.resize(LCD_WIDTH * LCD_HEIGHT);
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
    // Second step of drawing a scanline, VRAM not accessible by CPU
        if (clock >= 172) {
            clock -= 172;
            // At end of scanline, draw and switch to horizontal blank mode
            update_color_palettes();
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
                memory->set_interrupt(interrupt::VBLANK_bit);
                change_mode(VBLANK);
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
                u8 int_request = memory->read(reg::IF);
                memory->write(reg::IF, utils::reset(int_request, interrupt::VBLANK_bit));
                change_mode(OAM);
            }
        }
        break;    
    }
    update_STAT_register();
}

void GPU::update_STAT_register()
{
    // set LCDSTAT interrupt request if internal signal goes from 0 to 1
    bool prev_sig = stat_irq_signal;
    u8 stat = memory->read(reg::STAT);

    u8 coincidence_enable = 1 << 6;
    u8 oam_enable = 1 << 5;
    u8 vblank_enable = 1 << 4;
    u8 hblank_enable = 1 << 3;
    u8 coincidence_set = 1 << 2;

    stat_irq_signal = ((stat & coincidence_enable) && (stat & coincidence_set)) ||
                      ((stat & hblank_enable) && mode == HBLANK) ||
                      ((stat & oam_enable) && mode == OAM) ||
                      ((stat & (vblank_enable | oam_enable)) && mode == VBLANK);
    
    if (!prev_sig && stat_irq_signal) {
        memory->set_interrupt(interrupt::LCDSTAT_bit);
    }
}

void GPU::change_mode(Mode m)
{
    mode = m;
    // TODO - If LCD is off, set to 0
    STAT_reg = (STAT_reg & ~3) | (int)mode;
}

void GPU::increment_line()
{
    line++;
    memory->write(reg::LY, line);
    bool coincidence_flag = memory->read(reg::LYC) == memory->read(reg::LY);
    STAT_reg = utils::set_cond(STAT_reg, 2, coincidence_flag);
}

void GPU::update_color_palettes()
{
    u8 bgp = memory->read(reg::BGP);
    u8 obp1 = memory->read(reg::OBP1);
    u8 obp0 = memory->read(reg::OBP0);
    for (int i = 0; i < 4; i++) {
        bg_palette[i] = (bgp >> (2*i)) & 3;
        sprite_palette[0][i] = (obp0 >> (2*i)) & 3;
        sprite_palette[1][i] = (obp1 >> (2*i)) & 3;
    }
}

u8 GPU::read_pixel(std::vector<u8>::iterator &tile_data, int x, int y, bool invert_y, bool invert_x)
{
    int byte_ind = 2 * (invert_y ? TILE_DIM - 1 - y : y);
    u8 lsb = (tile_data[byte_ind] >> (invert_x ? x : TILE_DIM - 1 - x)) & 1;
    u8 msb = (tile_data[byte_ind + 1] >> (invert_x ? x: TILE_DIM - 1 - x)) & 1;
    return ((msb << 1) | lsb) & 3;
}

void GPU::draw_scanline()
{
    update_LCD_control();
    draw_background();
    draw_sprites();
    draw_window();
}

void GPU::draw_pixel(int x, int y, int color)
{
    // opengl texture coordinates are inverted relative to screen
    screen_texture[(LCD_WIDTH * (LCD_HEIGHT - 1 - y)) + x] = color;
}

void GPU::draw_background()
{
    int x = memory->read(reg::SCROLLX);
    int y = memory->read(reg::SCROLLY);
    auto vram = memory->video_RAM.begin() + LCD_control.tile_data_addr - VRAM_ADDR;
    for (int i = 0; i < LCD_WIDTH; i++) {
        int pixel_bg_coord_x = (x + i) % BACKGROUND_DIM;
        int pixel_bg_coord_y = (y + line) % BACKGROUND_DIM;

        int tile_map_x = pixel_bg_coord_x / TILE_DIM;
        int tile_map_y = pixel_bg_coord_y / TILE_DIM;
        int tile_map_index = (TILE_MAP_DIM * tile_map_y) + tile_map_x; 

        int tile_index = memory->read(LCD_control.bg_tile_map_addr + tile_map_index);        
        if (LCD_control.signed_tile_map) {
            tile_index = (i8)tile_index;
        }
        int pixel_tile_coord_x = pixel_bg_coord_x % TILE_DIM;
        int pixel_tile_coord_y = pixel_bg_coord_y % TILE_DIM;

        auto tile_data = vram + (BYTES_PER_TILE * tile_index);
        int color = read_pixel(tile_data, pixel_tile_coord_x, pixel_tile_coord_y, false, false);
        draw_pixel(i, line, bg_palette[color]);
    }
}

void GPU::draw_sprites()
{
    if (!LCD_control.enable_sprites) {
        return;
    }

    std::vector<u8>::iterator sprite_data = memory->sprite_attribute_table.begin();
    int sprite_size = (LCD_control.double_sprite_height ? 2 : 1) * TILE_DIM;

    std::vector<std::pair<int, int>> sprites;
    // Figure out which sprites are on the current scan line
    for (int i = 0; i < 40; i++) {
        int byte_ind = 4 * i;
        int y_pos = sprite_data[byte_ind] - 16;
        int x_pos = sprite_data[byte_ind + 1] - 8;
        if (x_pos == -8 || x_pos >= 160 || !(line >= y_pos && line < y_pos + sprite_size)) {
            continue;
        }
        sprites.emplace_back(x_pos, i);
    }
    // Sort sprites by x position
    sort(sprites.begin(), sprites.end());

    // Draw only first 10 sprites 
    int s = std::max((int)(sprites.size() - 10), 0);
    for (auto it = sprites.rbegin() + s; it < sprites.rend(); it++) {
        int byte_ind = it->second * 4;

        int y_pos = sprite_data[byte_ind] - 16;
        int x_pos = sprite_data[byte_ind + 1] - 8;

        u8 tile_num = sprite_data[byte_ind + 2];
        int flags = sprite_data[byte_ind + 3];

        bool behind_bg = utils::bit(flags, 7);
        bool flip_y = utils::bit(flags, 6);
        bool flip_x = utils::bit(flags, 5);
        bool palette_num = utils::bit(flags, 4);

        for (int i = std::max(0, x_pos); i < std::min(LCD_WIDTH, x_pos + TILE_DIM); i++) {
            int pixel_x = i - x_pos;
            int pixel_y = line - y_pos;
            int color;
            if (LCD_control.double_sprite_height) {
                u8 upper_tile_index = tile_num & 0xfe;
                u8 lower_tile_index = tile_num | 1;
                bool upper = pixel_y < 8;
                if (upper) {
                    tile_num = flip_y ? lower_tile_index : upper_tile_index;
                }
                else {
                    tile_num = flip_y ? upper_tile_index : lower_tile_index;
                    pixel_y -= 8;
                }
                auto tile_addr = memory->video_RAM.begin() + (BYTES_PER_TILE * tile_num);
                color = read_pixel(tile_addr, pixel_x, pixel_y, flip_y, flip_x);
            }
            else {
                auto tile_addr = memory->video_RAM.begin() + (BYTES_PER_TILE * tile_num);
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
    int window_x = memory->read(reg::WX) - 7;
    int window_y = line - memory->read(reg::WY);
    if (window_y < 0) {
        return;
    }
    auto vram = memory->video_RAM.begin() + LCD_control.tile_data_addr - VRAM_ADDR;
    for (int i = std::max(window_x, 0); i < LCD_WIDTH; i++) {
        int tile_map_x = (i - window_x) / TILE_DIM;
        int tile_map_y = window_y / TILE_DIM;
        int tile_map_index = (TILE_MAP_DIM * tile_map_y) + tile_map_x; 

        int tile_index = memory->read(LCD_control.win_tile_map_addr + tile_map_index);        
        if (LCD_control.signed_tile_map) {
            tile_index = (i8)tile_index;
        }
        int pixel_tile_coord_x = (i - window_x) % TILE_DIM;
        int pixel_tile_coord_y = window_y % TILE_DIM;

        auto tile_data = vram + (BYTES_PER_TILE * tile_index);
        int color = read_pixel(tile_data, pixel_tile_coord_x, pixel_tile_coord_y, false, false);
        draw_pixel(i, line, bg_palette[color]);
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
void GPU::update_LCD_control()
{
    u8 byte = memory->read(reg::LCDC);
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