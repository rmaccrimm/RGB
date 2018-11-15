#include "mmu.h"
#include "utils.h"
#include "registers.h"

typedef Register8bit r8;

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
    if (enable_boot_rom && addr <= 0xff && !utils::bit(read_reg(0xff50), 1)) {
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

        if (addr >= 0xff30 && addr <= 0xff3f) { 
        // Wave pattern RAM
        
                if (write_operation) 
                    wave_pattern_RAM[addr - 0xff30] = data;
                else
                    return_val = wave_pattern_RAM[addr - 0xff30];
        }
        else {
            if (write_operation) {
                if (io_used[addr - 0xff00])
                    write_reg(addr, data);
            }
            else 
                return_val = io_used[addr - 0xff00] ? read_reg(addr) : 0xff;
        }
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
            write_reg(addr, data);
        else
            return_val = read_reg(addr);
    }
}

u8 Memory::read_reg(u16 addr) 
{
    switch(addr) 
    {
    case reg::P1: 
    {
        bool select_dpad = !utils::bit(io_registers[addr].value(), 4);
        // io_registers[addr].write(joypad->get_state(select_dpad) & 0xf);
        // return io_registers[addr].value();
        return (3 << 6) | joypad->get_state(select_dpad);
    }
    default:
        return io_registers[addr].value();
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
        io_registers[addr].write(data);
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

// This seems dangerous and should definitely be removed later
std::vector<u8>::iterator Memory::get_vram_ptr(u16 addr)
{
    return video_RAM.begin() + (addr - 0x8000);
}

void Memory::init_registers()
{
    /* Registers with unused/read-only bits or special behaviour or read/write
    */
    io_registers[reg::P1] = r8(0b11000000);
    io_registers[reg::SB] = r8();
    io_registers[reg::SC] = r8(0b01111110);
    // hidden lower byte of timer
    io_registers[0xff03] = r8(0b11111111, 0b11111111);
    io_registers[reg::DIV] = r8();
    io_registers[reg::TIMA] = r8();
    io_registers[reg::TMA] = r8();
    io_registers[reg::TAC] = r8(0b11111000);
    io_registers[reg::IF] = r8(0b11100000);
    io_registers[reg::NR10] = r8(0b10000000);
    io_registers[reg::NR11] = r8(0b00111111);
    io_registers[reg::NR12] = r8();
    io_registers[reg::NR13] = r8();
    io_registers[reg::NR14] = r8(0b10111111);
    io_registers[reg::NR21] = r8(0b00111111);
    io_registers[reg::NR22] = r8();
    io_registers[reg::NR23] = r8();
    io_registers[reg::NR24] = r8(0b10111111);
    io_registers[reg::NR30] = r8(0b01111111);
    io_registers[reg::NR31] = r8();
    io_registers[reg::NR32] = r8(0b10011111);
    io_registers[reg::NR33] = r8();
    io_registers[reg::NR34] = r8(0b10111111);
    io_registers[reg::NR41] = r8(0b11000000);
    io_registers[reg::NR42] = r8();
    io_registers[reg::NR43] = r8();
    io_registers[reg::NR44] = r8(0b10111111);
    io_registers[reg::NR50] = r8();
    io_registers[reg::NR51] = r8();
    io_registers[reg::NR52] = r8(0b01110000);
    io_registers[reg::LCDC] = r8();
    io_registers[reg::SCROLLY] = r8();
    io_registers[reg::SCROLLX] = r8();
    io_registers[reg::LY] = r8();
    io_registers[reg::LYC] = r8();
    io_registers[reg::DMA] = r8();
    io_registers[reg::BGP] = r8();
    io_registers[reg::OBP0] = r8();
    io_registers[reg::OBP1] = r8();    
    io_registers[reg::WY] = r8();
    io_registers[reg::WX] = r8();
    io_registers[reg::STAT] = r8(0b10000000, 0b00000111);
    // boot-rom enable
    io_registers[0xff50] = r8();

    io_used.resize(0x80, false);
    for (auto p: io_registers) {
        io_used[p.first - 0xff00] = true;
    }
}