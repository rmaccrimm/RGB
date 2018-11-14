#include "register8bit.h"

Register8bit::Register8bit(): byte(0), read_mask(0), write_mask(0) {}

Register8bit::Register8bit(u8 unused): byte(0), read_mask(unused), write_mask(0) {}

Register8bit::Register8bit(u8 unused, u8 read_only): 
    byte(0), read_mask(unused), write_mask(read_only) {}

void Register8bit::set(u8 x) { byte = x; }

void Register8bit::write(u8 x) 
{ 
    // bits which are set in write_mask are ignored in x
    byte = (x & ~write_mask) | (byte & write_mask); 
}

u8 Register8bit::value() const 
{ 
    // bits in read_mask are always 1
    return byte | read_mask; 
}

void Register8bit::increment() { byte++; }

void Register8bit::decrement() { byte--; }

void Register8bit::add(u8 x) { byte += x; }

void Register8bit::sub(u8 x) { byte -= x; }


