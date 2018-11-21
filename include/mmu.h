#ifndef MMU_H
#define MMU_H

#include "definitions.h"
#include "joypad.h"
#include "cartridge.h"
#include <unordered_map>
#include <iterator>
#include <string>


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

private:
    Joypad *joypad;
    Cartridge *cartridge;

    std::vector<u8> boot_ROM;
    std::vector<u8> video_RAM;
    std::vector<u8> internal_RAM;
    std::vector<u8> sprite_attribute_table;
    std::vector<u8> wave_pattern_RAM;
    std::vector<u8> high_RAM;

    std::vector<u8> io_registers;
    std::vector<u8> io_read_masks;
    std::vector<u8> io_write_masks;

    u8 ie_reg;

    bool enable_boot_rom;
    bool enable_break_pt;
    bool paused;
    bool vram_updated;
    bool reset_clock;

    u16 break_pt;

    void init_registers();

    void write_reg(u16 addr, u8 data);

    u8 read_reg(u16 addr);

    std::vector<u8>::iterator get_vram_ptr(u16 addr);

    u8& get_mem_reference(u16 addr);

    void dma_transfer(u8 src);
};

#endif