#ifndef MMU_H
#define MMU_H

#include "definitions.h"
#include "register16bit.h"
#include "joypad.h"

class Memory
{
    friend class GPU;
    
public:
    Memory(Joypad *pad, bool enable_boot = 0);
    void write(u16 addr, u8 data);
    u8 read(u16 addr);
    
    void set_flags(u16 addr, u8 mask, bool b);
    void load_cart(const char *file_path, size_t offset = 0x100);
    void load_boot(const char *file_path);
    void load(u8 data[], size_t offset, size_t size);
    void set_access_break_pt(u16 addr);
    void clear_access_break_pt();
    bool pause();

private:
    // For accessing VRAM more quickly
    u8* get_mem_ptr(u16 addr);    

    u8 mem[0x10000];
    u8 boot_rom[0x100];
    Joypad *joypad;
    bool enable_boot_rom;
    bool enable_break_pt;
    bool paused;
    u16 break_pt;

};

#endif