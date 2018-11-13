#include "register8bit.h"

Register8bit::Register8bit(): byte(0), write_mask(0xff) {}

Register8bit::Register8bit(u8 w_mask): byte(0), write_mask(w_mask) {}

u8 Register8bit::value() const { return byte; }

void Register8bit::set(u8 x) { byte = x; }

void Register8bit::increment() { byte++; }

void Register8bit::decrement() { byte--; }

void Register8bit::add(u8 x) { byte += x; }

void Register8bit::sub(u8 x) { byte -= x; }

void Register8bit::write(u8 x) 
{ 
    byte = (x & write_mask) | (byte & ~write_mask); 
}
