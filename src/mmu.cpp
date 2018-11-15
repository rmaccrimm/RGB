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
    joypad(pad), 
    cartridge(cart), 
    mem{0}, 
    enable_boot_rom(enable_boot), 
    enable_break_pt(false), 
    paused(false), 
    vram_updated(false),
    reset_clock(false)
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
        if (addr <= 0xfdff) { // Echo RAM 
            return mem[addr - 0x2000];
        }
        else if (addr <= 0xfe9f) { // OAM
            return mem[addr];
        }
        else if (addr <= 0xfeff) { // unusable memory
            return 0xff;
        }
        else if (addr <= 0xff7f || addr == reg::IE) { // I/O Registers
            if (mem_registers.find(addr) != mem_registers.end()) {
                return read_reg(addr);
            }
            else if (addr >= 0xff30 && addr <= 0xff3f) { // Wave pattern RAM
                return mem[addr];
            }
            else {
                return 0xff;
            }
        }
        else { //High RAM
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
        if (addr <= 0xfdff) { // Echo RAM
            mem[addr - 0x2000] = data;
        }
        else if (addr <= 0xfe9f) { // OAM
            vram_updated = true;
            mem[addr] = data;
        }
        else if (addr <= 0xfeff) { // unusable memory
            break;            
        }
        else if (addr <= 0xff7f || addr == reg::IE) { // I/O registers
            if (mem_registers.find(addr) != mem_registers.end()) {
                write_reg(addr, data);
            }
            else if (addr >= 0xff30 && addr <= 0xff3f) { // Wave pattern RAM
                    mem[addr] = data;
            }
            else {
                break;
            }
        }
        else { // High RAM
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
    case reg::P1: 
    {
        bool select_dpad = !utils::bit(mem_registers[addr].value(), 4);
        return (3 << 6) | joypad->get_state(select_dpad);
    }
    default:
        return mem_registers[addr].value();
    }
}

void Memory::write_reg(u16 addr, u8 data)
{
    switch(addr)
    {
    case reg::DIV:
        reset_clock = true;
        break;
    default:
        mem_registers[addr].write(data);
    }
}

void Memory::set_interrupt(int interrupt_bit)
{
    u8 int_request = read(reg::IF);
    write(reg::IF, utils::set(int_request, interrupt_bit));
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

std::vector<u8>::iterator Memory::get_mem_ptr(u16 addr) { return mem.begin() + addr; }

void Memory::init_registers()
{
    mem_registers[reg::P1] = Register8bit(0b11000000);
    mem_registers[reg::SB] = Register8bit();
    mem_registers[reg::SC] = Register8bit(0b01111110);
    // hidden lower byte of timer
    mem_registers[0xff03] = Register8bit(0b11111111, 0b11111111);
    mem_registers[reg::DIV] = Register8bit();
    mem_registers[reg::TIMA] = Register8bit();
    mem_registers[reg::TMA] = Register8bit();
    mem_registers[reg::TAC] = Register8bit(0b11111000);
    mem_registers[reg::IF] = Register8bit(0b11100000);
    mem_registers[reg::NR10] = Register8bit(0b10000000);
    mem_registers[reg::NR11] = Register8bit(0b00111111);
    mem_registers[reg::NR12] = Register8bit();
    mem_registers[reg::NR13] = Register8bit();
    mem_registers[reg::NR14] = Register8bit(0b10111111);
    mem_registers[reg::NR21] = Register8bit(0b00111111);
    mem_registers[reg::NR22] = Register8bit();
    mem_registers[reg::NR23] = Register8bit();
    mem_registers[reg::NR24] = Register8bit(0b10111111);
    mem_registers[reg::NR30] = Register8bit(0b01111111);
    mem_registers[reg::NR31] = Register8bit();
    mem_registers[reg::NR32] = Register8bit(0b10011111);
    mem_registers[reg::NR33] = Register8bit();
    mem_registers[reg::NR34] = Register8bit(0b10111111);
    mem_registers[reg::NR41] = Register8bit(0b11000000);
    mem_registers[reg::NR42] = Register8bit();
    mem_registers[reg::NR43] = Register8bit();
    mem_registers[reg::NR44] = Register8bit(0b10111111);
    mem_registers[reg::NR50] = Register8bit();
    mem_registers[reg::NR51] = Register8bit();
    mem_registers[reg::NR52] = Register8bit(0b01110000);
    mem_registers[reg::LCDC] = Register8bit();
    mem_registers[reg::SCROLLY] = Register8bit();
    mem_registers[reg::SCROLLX] = Register8bit();
    mem_registers[reg::LY] = Register8bit();
    mem_registers[reg::LYC] = Register8bit();
    mem_registers[reg::DMA] = Register8bit();
    mem_registers[reg::BGP] = Register8bit();
    mem_registers[reg::OBP0] = Register8bit();
    mem_registers[reg::OBP1] = Register8bit();    
    mem_registers[reg::WY] = Register8bit();
    mem_registers[reg::WX] = Register8bit();
    mem_registers[reg::STAT] = Register8bit(0b10000000, 0b00000111);
    mem_registers[reg::IE] = Register8bit();
    // Additional unlisted registers
    mem_registers[0xff72] = Register8bit();
    mem_registers[0xff73] = Register8bit();
    mem_registers[0xff75] = Register8bit(0b10001111);
    mem_registers[0xff76] = Register8bit(0, 0b11111111);
    mem_registers[0xff77] = Register8bit(0, 0b11111111);
}