#include "debug.h"
#include <iostream>
#include <string>
#include "gpu.h"

u16 DEBUG::get_break_point()
{
    std::cout << "Enter program break point (inclusive): ";
    u16 bp;
    std::cin >> bp;
    return bp;
}

char DEBUG::debug_menu()
{
    const std::string prompt =
        std::string("memory range:\t\tm\n") +
        "register contents:\tr\n" +
        "continue:\t\tc\n" +
        "continue until:\t\tu\n" +
        "quit:\t\t\tq\n\n";
    std::cout << prompt; 
    std::string choice;
    char c;
    do {
        std::cout << "choice: ";
        getline(std::cin, choice);
        c = tolower(choice[0]);
    } while (choice.size() >  1 || (c != 'm' && c != 'r' && c != 'c' &&
                                    c != 'u' && c != 'q'));
    return c;
}

void DEBUG::setup_stripe_pattern(u8 *memory)
{
    u8 lcdc = 0;
    // enable lcd
    lcdc |= 1 << 7;
    // tile data 1, unsigned
    lcdc |= 1 << 4;
    // enable bg
    lcdc |= 1;
    
    memory[GPU::LCDC] = lcdc;
    memory[GPU::SCROLLX] = 0;
    memory[GPU::SCROLLY] = 0;

    u8 colors[] = {0xff, 0xaa, 0x55, 0};
    for (u8 i = 0; i < 64; i++) {
        memory[GPU::TILE_DATA_1 + i] = colors[i/16];
    } 
    for (u8 i = 0; i < 32; i++) {
        for (u8 j = 0; j < 32; j++) {
            memory[GPU::TILE_MAP_0 + (32 * i) + j] = i % 4;
        }
    }
}

void DEBUG::setup_dot_pattern(u8 *memory)
{
    u8 lcdc = 0;
    // enable lcd
    lcdc |= 1 << 7;
    // tile data 1, unsigned
    lcdc |= 1 << 4;
    // enable bg
    lcdc |= 1;
    
    memory[GPU::LCDC] = lcdc;
    memory[GPU::SCROLLX] = 0;
    memory[GPU::SCROLLY] = 0;

    u8 tile0[] = {0x22, 0x22, 0x88, 0x88, 0x22, 0x22, 0x88, 0x88,
                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    u8 tile1[] = {0xdd, 0xdd, 0x77, 0x77, 0xdd, 0xdd, 0x77, 0x77, 
                  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    u8 tile2[] = {0x99, 0x99, 0x66, 0x66, 0x99, 0x99, 0x66, 0x66, 
                  0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
    u8 tile3[] = {0x33, 0x33, 0xcc, 0xcc, 0x33, 0x33, 0xcc, 0xcc,
                  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
    memcpy(&memory[GPU::TILE_DATA_1], tile0, 16);
    memcpy(&memory[GPU::TILE_DATA_1 + 16], tile1, 16);
    memcpy(&memory[GPU::TILE_DATA_1 + 32], tile2, 16);
    memcpy(&memory[GPU::TILE_DATA_1 + 48], tile3, 16);
    for (u8 i = 0; i < 32; i++) {
        for (u8 j = 0; j < 32; j++) {
            memory[GPU::TILE_MAP_0 + (32 * i) + j] = i % 4;
        }
    }
}

void DEBUG::print_tile_map(u8 *memory, bool map)
{
    u16 tile_map;
    if (map) {
        tile_map = GPU::TILE_MAP_1;
    }
    else {
        tile_map = GPU::TILE_MAP_0;
    }
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            std::cout << (int)memory[tile_map + 32*i + j] << ' ';
        }
        std::cout << std::endl;
    }
}
