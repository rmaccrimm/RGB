#ifndef MMU_H
#define MMU_H

#include <set>
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
    void set_access_break_pt(u16 addr);
    void clear_access_break_pt();
    bool pause();

	bool vram_updated;

private:
    // For accessing VRAM more quickly
    u8* get_mem_ptr(u16 addr);
    void write_reg(u16 addr, u8 data);
    u8 read_reg(u16 addr);
    void init_registers();

    Joypad *joypad;
    Cartridge *cartridge;
    std::vector<u8> mem;
    std::set<u16> special_registers;

    bool enable_boot_rom;
    bool enable_break_pt;
    bool paused;
    u16 break_pt;

    std::vector<u8> boot_rom;
    //u8 mem[0x10000];
};

#endif