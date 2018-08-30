#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "definitions.h"

namespace interrupt 
{
    static const u8 VBLANK  = 0;
    static const u8 LCDSTAT = 1;
    static const u8 TIMER   = 2;
    static const u8 SERIAL  = 3;
    static const u8 JOYPAD  = 4;
};

#endif