#ifndef MMU_H
#define MMU_H

#include "definitions.h"
#include "register16bit.h"
#include "joypad.h"
#include "cartridge.h"

class Memory
{
    friend class GPU;
    
public:
    Memory(Cartridge *cart, Joypad *pad, bool enable_boot = 0);
    void write(u16 addr, u8 data);
    u8 read(u16 addr);
    
    void set_flags(u16 addr, u8 mask, bool b);
    void load_boot(std::string s);
    void load(u8 data[], size_t offset, size_t size);
    void set_access_break_pt(u16 addr);
    void clear_access_break_pt();
    bool pause();

	bool vram_updated;

private:
    // For accessing VRAM more quickly
    u8* get_mem_ptr(u16 addr);
    Cartridge *cartridge;

    u8 mem[0x10000];
    std::vector<u8> boot_rom;
    Joypad *joypad;
    bool enable_boot_rom;
    bool enable_break_pt;
    bool paused;
    u16 break_pt;
};

#endif