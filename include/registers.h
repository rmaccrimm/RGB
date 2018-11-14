#ifndef REGISTERS_H
#define REGISTERS_H

#include "definitions.h"

namespace reg
{
    // VRAM
    static const u16 TILE_MAP_0     = 0x9800; // unsigned
    static const u16 TILE_MAP_1     = 0x9c00; // signed
    static const u16 TILE_DATA_0    = 0x9000; // signed tile index
    static const u16 TILE_DATA_1    = 0x8000; // unsigned tile index
    static const u16 OAM            = 0xfe00; // sprites
    // Joypad
    static const u16 JOYP       = 0xff00;
    // Serial transfer data
    static const u16 SB         = 0xff01;
    // SIO control
    static const u16 SC         = 0xff02;
    // Divider
    static const u16 DIV        = 0xff04;
    // Timer
    static const u16 TIMA       = 0xff05;
    // Timer modulo
    static const u16 TMA        = 0xff06;
    static const u16 TAC        = 0xff07;
    // Interrupt request
    static const u16 IF         = 0xff0f;
    static const u16 NR10       = 0xff10;
    static const u16 NR11       = 0xff11;
    static const u16 NR12       = 0xff12;
    static const u16 NR14       = 0xff14;
    static const u16 NR21       = 0xff16;
    static const u16 NR22       = 0xff17;
    static const u16 NR24       = 0xff19;
    static const u16 NR30       = 0xff1a;
    static const u16 NR31       = 0xff1b;
    static const u16 NR32       = 0xff1c;
    static const u16 NR33       = 0xff1e;
    static const u16 NR41       = 0xff20;
    static const u16 NR42       = 0xff21;
    static const u16 NR43       = 0xff22;
    static const u16 NR44       = 0xff23;
    static const u16 NR50       = 0xff24;
    static const u16 NR51       = 0xff25;
    static const u16 NR52       = 0xff26;
    // LCD control
    static const u16 LCDC       = 0xff40;
    // LCD status
    static const u16 STAT       = 0xff41;
    // Screen position
    static const u16 SCROLLY    = 0xff42;
    static const u16 SCROLLX    = 0xff43;
    // Current line
    static const u16 LY         = 0xff44;
    static const u16 LYC        = 0xff45;
    static const u16 DMA        = 0xff46;    
    // Background color palette
    static const u16 BGP        = 0xff47;
    // Sprite color palettes
    static const u16 OBP0       = 0xff48;
    static const u16 OBP1       = 0xff49;
    // Window position
    static const u16 WY         = 0xff4a;
    static const u16 WX         = 0xff4b;
    // Interrupt enable
    static const u16 IE         = 0xffff;    
}

#endif
