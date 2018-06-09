#include "include/memory.h"

Memory::Memory() : mem{0} {}

void Memory::write(u16 addr, u8 data)
{
    mem[addr] = data;
}

u8 Memory::read(u16 addr) const
{
    return mem[addr];
}