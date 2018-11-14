#ifndef MMU_H
#define MMU_H

#include <map>
#include <iterator>
#include "definitions.h"
#include "register16bit.h"
#include "joypad.h"
#include "cartridge.h"

class Memory
{
    friend class GPU;
    friend class Processor;
    
public:
    Memory(Cartridge *cart, Joypad *pad, bool enable_boot = 0);

    void write(u16 addr, u8 data);

    u8 read(u16 addr);
    
    void set_interrupt(int interrupt_bit);

    void load_boot(std::string s);

    void set_access_break_pt(u16 addr);

    void clear_access_break_pt();

    bool pause();

	bool vram_updated;

private:
    void write_reg(u16 addr, u8 data);
    u8 read_reg(u16 addr);
    void init_registers();
    
    std::vector<u8>::iterator get_mem_ptr(u16 addr);

    Joypad *joypad;
    Cartridge *cartridge;

    std::vector<u8> mem;
    std::map<u16, Register8bit> mem_registers;
    std::vector<u8> boot_rom;

    bool enable_boot_rom;
    bool enable_break_pt;
    bool paused;
    u16 break_pt;
};

#endif