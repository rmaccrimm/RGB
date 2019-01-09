#include "interrupts.h"
#include "util.h"

const u8 Interrupts::VBLANK_bit  = 0;
const u8 Interrupts::LCDSTAT_bit = 1;
const u8 Interrupts::TIMER_bit   = 2;
const u8 Interrupts::SERIAL_bit  = 3;
const u8 Interrupts::JOYPAD_bit  = 4;

Interrupts::Interrupts() : reg(0) {}

u8 Interrupts::read()
{
    // upper 3 bits are always set
    return reg | 0b11100000;
}

void Interrupts::write(u8 data)
{
    reg = data;
}

void Interrupts::set(int bit)
{
    reg = utils::set(reg, bit);
}

void Interrupts::clear(int bit)
{
    reg = utils::reset(reg, bit);
}
