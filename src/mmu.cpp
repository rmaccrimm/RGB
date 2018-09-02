#include "mmu.h"
#include "functions.h"
#include "registers.h"
#include <cstring>
#include <iostream>

Memory::Memory(Joypad *pad, bool enable_boot) : mem{0}, joypad(pad), enable_boot_rom(enable_boot),
    enable_break_pt(false), paused(false) {}

void Memory::write(u16 addr, u8 data)
{
    if (enable_break_pt && addr == break_pt) {
        paused = true;
    }

    if (addr >= 0xfea0 && addr <= 0xfeff) { // unusable memory
        return;
    }
    /*else if (addr == reg::DIV) {
        // writing any value to DIV writes 0 and resets system counter
        mem[addr] = 0;
        clock_counter->set(0);
    }*/
    else {
        mem[addr] = data;
    }
}

u8 Memory::read(u16 addr) 
{
    if (addr <= 0x100) {
        if (!(mem[0xff50] & 1) && enable_boot_rom) { // not sure about this check
            return boot_rom[addr];
        } 
        else {
            return mem[addr];
        }
    }
    else if (addr >= 0xfea0 && addr <= 0xfeff) { // unusable memory
        return 0xff;
    }
    else if (addr == reg::IF) {

    }
    else if (addr == 0xff00) {
        // if bit 4 is reset select dpad, assume dpad/buttons mutually exclusive
        bool select_dpad = (mem[addr] & (1 << 4)) == 0;
        return joypad->get_state(select_dpad);
    }
    return mem[addr];
}

void Memory::set_flags(u16 addr, u8 mask, bool b)
{
    if (b) {
        mem[addr] |= mask;
    } else {
        mem[addr] &= ~mask;
    }
}

void Memory::load_cart(const char *file_path, size_t offset)
{
    utils::load_rom(mem, offset, file_path);
}

void Memory::load_boot(const char *file_path)
{
    utils::load_rom(boot_rom, 0, file_path);
}

void Memory::load(u8 data[], size_t offset, size_t size)
{
    memcpy(mem + offset, data, size);
}

void Memory::set_access_break_pt(u16 addr) 
{ 
    enable_break_pt = true;
    break_pt = addr; 
}

void Memory::clear_access_break_pt() { enable_break_pt = false; }

bool Memory::pause() { return paused; }