#include "functions.h"
#include <fstream>
#include <vector>
#include <iostream>
#include "gpu.h"
using namespace std;

void load_rom(u8 memory[], const char *path)
{
    size_t file_size;
    ifstream ifs(path, ios_base::in | ios_base::binary);
    ifs.seekg(0, ios_base::end);
    file_size = ifs.tellg();
    ifs.seekg(0, ios_base::beg);
    if (!ifs.good()) {
        cout << "Error reading file: " << path << endl;
    }
    char *buff = new char[file_size];
    ifs.read(buff, file_size);
    ifs.close();
    std::memcpy(memory, buff, file_size);
    delete buff;
}

void set(bool &b) 
{ 
    b = true; 
}

void reset(bool &b) 
{ 
    b = false; 
}

// Check for carry from bit 3 to bit 4 in a + b
bool half_carry_add(u16 a, u16 b) 
{
    return (((a & 0xf) + (b & 0xf)) & 0x10) == 0x10;
}

// Check for carry from bit 4 to 3 in a - b 
bool half_carry_sub(u16 a, u16 b)
{
    return ((a & 0xf) - (b & 0xf)) < 0;
}

bool full_carry_add(u16 a, u16 b)
{
    return (((a & 0xff) + (b & 0xff)) & 0x100) == 0x100;
}

bool full_carry_sub(u16 a, u16 b)
{
    return ((a & 0xff) - (b & 0xff)) < 0;
}

void setup_stripe_pattern(u8 *memory)
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

