#include "mmu.h"
#include "util.h"
#include "registers.h"
#include <map>
#include <cassert>

Memory::Memory(Cartridge *cart, Joypad *pad, bool enable_boot) : 
    joypad(pad), 
    cartridge(cart), 
    enable_boot_rom(enable_boot),
    enable_break_pt(false), 
    paused(false), 
    vram_updated(false),
    reset_clock(false)
{
    video_RAM.resize(0x2000, 0); // 8kB
    internal_RAM.resize(0x2000, 0); 
    sprite_attribute_table.resize(0xa0, 0);
    wave_pattern_RAM.resize(0x10, 0);
    high_RAM.resize(0x7f, 0);
    init_registers();
}

u8 Memory::read(u16 addr) 
{
    u8 r;
    map_memory(addr, 0, false, r);
    return r;
}

void Memory::write(u16 addr, u8 data)
{   
    if (enable_break_pt && addr == break_pt) 
        paused = true; 

    u8 r;
    map_memory(addr, data, true, r);
}

void Memory::map_memory(u16 addr, u8 data, bool write_operation, u8 &return_val)
{
    if (enable_boot_rom && addr <= 0xff) { 
    // Boot ROM

        if (!write_operation)
            return_val = boot_ROM[addr];
    }
    else if (addr <= 0x7fff) {
    // External ROM

        if (write_operation) 
            cartridge->write(addr, data);
        else
            return_val = cartridge->read(addr);
    }
    else if (addr >= 0x8000 && addr <= 0x9fff) {
    // VRAM

        if (write_operation) {
            vram_updated = true;
            video_RAM[addr - 0x8000] = data;
        }
        else 
            return_val = video_RAM[addr - 0x8000];
    }
    else if (addr >= 0xa000 && addr <= 0xbfff) {
    // External RAM

        if (write_operation) 
            cartridge->write(addr, data);
        else 
            return_val = cartridge->read(addr);
    }
    else if (addr >= 0xc000 && addr <= 0xdfff) {
    // Internal RAM

        if (write_operation)
            internal_RAM[addr - 0xc000] = data;
        else
            return_val = internal_RAM[addr - 0xc000];

    }
    else if (addr >= 0xe000 && addr <= 0xfdff) {
    // Echo RAM

        map_memory(addr - 0x2000, data, write_operation, return_val);
    }
    else if (addr >= 0xfe00 && addr <= 0xfe9f) {
    // OAM

        if (write_operation)
            sprite_attribute_table[addr - 0xfe00] = data;
        else
            return_val = sprite_attribute_table[addr - 0xfe00];
    }
    else if (addr >= 0xfea0 && addr <= 0xfeff) {
    // Unusable 

        if (!write_operation)
            return_val = 0xff;
    }
    else if (addr >= 0xff00 && addr <= 0xff7f) {
    // IO registers

        if (write_operation) {
            write_reg(addr, data);
        }
        else 
            return_val = read_reg(addr);
    }
    else if (addr >= 0xff80 && addr <= 0xfffe) {
    // High RAM

        if (write_operation) 
            high_RAM[addr - 0xff80] = data;
        else
            return_val = high_RAM[addr - 0xff80];
    }
    else if (addr == 0xffff) {
    // IE register

        if (write_operation)
            ie_reg = data;
        else
            return_val = ie_reg;
    }
}

u8 Memory::read_reg(u16 addr) 
{
    u8 reg_val = io_registers[addr - 0xff00] | io_read_masks[addr - 0xff00];
    switch(addr) 
    {
    case reg::P1: 
    {
        bool select_dpad = !utils::bit(reg_val, 4);
        return (3 << 6) | joypad->get_state(select_dpad);
    }
    default:
        return reg_val;
    }
}

void Memory::write_reg(u16 addr, u8 data)
{
    switch(addr)
    {
    case reg::DIV:
        reset_clock = true;
        break;
    case 0xff03:
        break;
    case reg::STAT:
        // bits 0 - 2 read-only
        io_registers[addr - 0xff00] = (data & ~7) | (io_registers[addr - 0xff00] & 7);
        break;
    case reg::DMA:
        dma_transfer(data);
        io_registers[addr - 0xff00] = data;
        break;
    case 0xff50:
        enable_boot_rom = false;
        break;
    default:
        io_registers[addr - 0xff00] = data;
    }
}

u8& Memory::get_mem_reference(u16 addr)
{
    switch (addr)
    {
    case reg::STAT:
    case reg::DIV:
        return io_registers[addr - 0xff00];
    default:
        assert(false);
    }
}

void Memory::set_interrupt(int interrupt_bit)
{
    u8 int_request = read(reg::IF);
    write(reg::IF, utils::set(int_request, interrupt_bit));
}

void Memory::load_boot(std::string file_path)
{
    utils::load_file(boot_ROM, file_path);
}

void Memory::set_access_break_pt(u16 addr) 
{ 
    enable_break_pt = true;
    break_pt = addr; 
}

void Memory::clear_access_break_pt() { enable_break_pt = false; }

bool Memory::pause() { return paused; }

void Memory::dma_transfer(u8 src)
{
    u16 start_addr = src << 8;
    for (int i = 0; i <= 0x9f; i++) {
        sprite_attribute_table[i] =  read(start_addr + i);
    }
}

void Memory::init_registers()
{
    /* Registers with unused/read-only bits or special behaviour or read/write
    */
    std::map<u16, u8> io;
    io[reg::P1] = 0b11000000;
    io[reg::SB] = 0;
    io[reg::SC] = 0b01111110;
    // hidden lower byte of timer
    io[0xff03] = 0b11111111;//, 0b11111111);
    io[reg::DIV] = 0;
    io[reg::TIMA] = 0;
    io[reg::TMA] = 0;
    io[reg::TAC] = 0b11111000;
    io[reg::IF] = 0b11100000;
    io[reg::NR10] = 0b10000000;
    io[reg::NR11] = 0b00111111;
    io[reg::NR12] = 0;
    io[reg::NR13] = 0;
    io[reg::NR14] = 0b10111111;
    io[reg::NR21] = 0b00111111;
    io[reg::NR22] = 0;
    io[reg::NR23] = 0;
    io[reg::NR24] = 0b10111111;
    io[reg::NR30] = 0b01111111;
    io[reg::NR31] = 0;
    io[reg::NR32] = 0b10011111;
    io[reg::NR33] = 0;
    io[reg::NR34] = 0b10111111;
    io[reg::NR41] = 0b11000000;
    io[reg::NR42] = 0;
    io[reg::NR43] = 0;
    io[reg::NR44] = 0b10111111;
    io[reg::NR50] = 0;
    io[reg::NR51] = 0;
    io[reg::NR52] = 0b01110000;
    io[reg::LCDC] = 0;
    io[reg::SCROLLY] = 0;
    io[reg::SCROLLX] = 0;
    io[reg::LY] = 0;
    io[reg::LYC] = 0;
    io[reg::DMA] = 0;
    io[reg::BGP] = 0;
    io[reg::OBP0] = 0;
    io[reg::OBP1] = 0;    
    io[reg::WY] = 0;
    io[reg::WX] = 0;
    io[reg::STAT] = 0b10000000;//, 0b00000111;
    // boot-rom enable
    io[0xff50] = 0b11111111;

    io_registers.resize(0x80, 0);
    io_read_masks.resize(0x80, 0);
    io_write_masks.resize(0x80, 0);

    io_write_masks[0xff03 - 0xff00] = 0xff;
    io_write_masks[reg::STAT - 0xff00] = 0x7;

    for (int i = 0xff00; i < 0xff80; i++) {
        auto p = io.begin();
        if ((p = io.find(i)) != io.end()) {
            io_read_masks[i - 0xff00] = p->second;
        }
        else if (!(i >= 0xff30 && i <= 0xff3f)) {
            io_read_masks[i - 0xff00] = 0xff;
        }
    }
}