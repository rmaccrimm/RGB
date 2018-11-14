#include "mmu.h"
#include "utils.h"
#include "registers.h"
#include "register8bit.h"
#include <cstring>
#include <iostream>
#include <cassert>
#include <algorithm>

typedef Register8bit r8;

Memory::Memory(Cartridge *cart, Joypad *pad, bool enable_boot) : 
    cartridge(cart), 
    mem{0}, 
    joypad(pad), 
    enable_boot_rom(enable_boot), 
    enable_break_pt(false), 
    paused(false), 
    vram_updated(false) 
{
    mem.resize(0x10000, 0);
    init_registers();
}

u8 Memory::read(u16 addr) 
{
    switch(addr / 0x1000) 
    {
    // Switchable ROM Bank
    case 0x0: 
        if (addr <= 0x100 && (!(mem[0xff50] & 1) && enable_boot_rom)) { 
            return boot_rom[addr];
        } 
    case 0x1:
    case 0x2:
    case 0x3:
    case 0x4:
    case 0x5:
    case 0x6:
    case 0x7: 
        return cartridge->read(addr);
    // VRAM        
    case 8: 
    case 9:
        return mem[addr];
    // Switchable RAM bank
    case 0xa: 
    case 0xb:
        return cartridge->read(addr);
    // Internal RAM
    case 0xc:
    case 0xd:
        return mem[addr];
    case 0xe:
    case 0xf:
        if (special_registers.find(addr) != special_registers.end()) {
            return read_reg(addr);
        }
        else if (addr <= 0xfdff) { // Echo RAM
            return mem[addr - 0x2000];
        }
        else if (addr <= 0xfe9f) { // OAM
            return mem[addr];
        }
        else if (addr <= 0xfeff) { // unusable memory
            return 0xff;
        }
        else if (addr >= 0xff00) { // I/O Registers + High RAM
            return mem[addr];
        }   
    default:
        std::cout << "Read from address " << addr << std::endl;
        assert(false);
        return 0xff;
    }
}

void Memory::write(u16 addr, u8 data)
{
    if (enable_break_pt && addr == break_pt) { 
        paused = true; 
    }

    switch(addr / 0x1000) {
    // Select RAM/ROM bank                 
    case 0x0: 
    case 0x1:
    case 0x2:
    case 0x3:
    case 0x4:
    case 0x5:
    case 0x6:
    case 0x7: 
        cartridge->write(addr, data);
        break;
    // VRAM        
    case 8: 
    case 9:
        vram_updated = true;
        mem[addr] = data;
        break;
    // Switchable RAM bank
    case 0xa: 
    case 0xb:
        cartridge->write(addr, data);
        break;
    // Internal RAM
    case 0xc:
    case 0xd:
        mem[addr] = data;
        break;
    case 0xe:
    case 0xf:
        if (special_registers.find(addr) != special_registers.end()) {
            write_reg(addr, data);
        }
        else if (addr <= 0xfdff) { // Echo RAM
            mem[addr - 0x2000] = data;
        }
        else if (addr <= 0xfe9f) { // OAM
            vram_updated = true;
            mem[addr] = data;
        }
        // 0xfea0-0xfeff not usable
        else if (addr >= 0xff00 ) {
            mem[addr] = data;
        }
        break;
    default:
        std::cout << "Wrote to address " << addr << std::endl;
        assert(false);
    }
}

u8 Memory::read_reg(u16 addr) 
{
    switch(addr)
    {
    case reg::JOYP: 
    {
        bool select_dpad = (mem[addr] & (1 << 4)) == 0;
        return joypad->get_state(select_dpad);
    }
    case reg::STAT: // bit 7 is always set
        return (1 << 7) | mem[addr];
    default:
        return mem[addr];
    }
}

void Memory::write_reg(u16 addr, u8 data)
{
    switch(addr)
    {
    case reg::DIV:
        mem[addr] = 0;
        break;
    case reg::STAT: // bits 0 - 2 read-only
        mem[addr] = (data & ~7) | (mem[addr] & 7); 
        break;
    default:
        mem[addr] = data;
    }
}

void Memory::set_flags(u16 addr, u8 mask, bool b)
{
    if (b) {
        mem[addr] |= mask;
    } else {
        mem[addr] &= ~mask;
    }
}

void Memory::load_boot(std::string file_path)
{
    utils::load_file(boot_rom, file_path);
}

void Memory::set_access_break_pt(u16 addr) 
{ 
    enable_break_pt = true;
    break_pt = addr; 
}

void Memory::clear_access_break_pt() { enable_break_pt = false; }

bool Memory::pause() { return paused; }

u8* Memory::get_mem_ptr(u16 addr) { return &mem.data()[addr]; }

void Memory::init_registers()
{
    std::vector<u16> r = {
        reg::DIV,
        reg::JOYP,
        reg::STAT
    };
    special_registers.insert(r.begin(), r.end());
}