#ifndef MEMORY_H
#define MEMORY_H

#include "definitions.h"

class Memory
{
public:
    Memory();
    void write(u16 addr, u8 data);
    u8 read(u16 addr) const;
    void load_cart(const char *file_path);
    void load(u8 data[], size_t offset, size_t size);

private:
    u8 mem[0x10000];
};

#endif