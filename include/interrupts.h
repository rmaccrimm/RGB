#ifndef GB_INTERRUPTS_H
#define GB_INTERRUPTS_H

#include "definitions.h"

class Interrupts 
{
public:
    Interrupts();

    u8 read();
    void write(u8 data);

    void set(int bit);
    void clear(int bit);

    static const u8 VBLANK_bit;
    static const u8 LCDSTAT_bit;
    static const u8 TIMER_bit;
    static const u8 SERIAL_bit;
    static const u8 JOYPAD_bit;

private:
    u8 reg;
};

#endif