#ifndef MMU_H
#define MMU_H

#include "definitions.h"
#include "register16bit.h"

class Memory
{
public:
    Memory(Register16bit *clock);
    void write(u16 addr, u8 data);
    u8 read(u16 addr) const;
    void set_flags(u16 addr, u8 mask, bool b);
    void load_cart(const char *file_path, size_t offset = 0x100);
    void load_cart(const char *file_path, size_t start, size_t offset);
    void load(u8 data[], size_t offset, size_t size);

private:
    u8 mem[0x10000];
    u8 boot_rom[0x100];
    Register16bit *clock_counter;
    bool enable_boot_rom;
};

#endif