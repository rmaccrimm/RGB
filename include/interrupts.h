#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "definitions.h"

namespace interrupt 
{
    static const u8 VBLANK_bit  = 0;
    static const u8 LCDSTAT_bit = 1;
    static const u8 TIMER_bit   = 2;
    static const u8 SERIAL_bit  = 3;
    static const u8 JOYPAD_bit  = 4;
};

#endif