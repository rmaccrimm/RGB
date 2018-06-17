#ifndef REGISTERS_H
#define REGISTERS_H

#include "definitions.h"

namespace reg
{
    // LCD control
    static const u16 LCDC       = 0xff40;
    // LCD status
    static const u16 STAT       = 0xff41;
    // Screen position
    static const u16 SCROLLY    = 0xff42;
    static const u16 SCROLLX    = 0xff43;
    // Current line
    static const u16 LY         = 0xff44;
    // Color palette
    static const u16 BGP        = 0xff47;
    // Window position
    static const u16 WY         = 0xff4a;
    static const u16 WX         = 0xff4b;

    // VRAM
    static const u16 TILE_MAP_0 = 0x9800;
    static const u16 TILE_MAP_1 = 0x9c00;
    static const u16 TILE_DATA_0 = 0x9000; // signed tile index
    static const u16 TILE_DATA_1 = 0x8000; // unsigned tile index
    
}

#endif REGISTERS_H