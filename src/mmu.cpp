#include "mmu.h"
#include "util.h"
#include "registers.h"
#include <map>
#include <cassert>

Memory::Memory(
    Interrupts *inter, Cartridge *cart, Joypad *pad, APU *audio, GPU *video,bool enable_boot) : 
    joypad(pad), 
    cartridge(cart), 
    apu(audio),
    gpu(video),
    interrupts(inter),
    enable_boot_rom(enable_boot),
    enable_break_pt(false), 
    paused(false), 
    vram_updated(false),
    reset_clock(false),
    audio_trigger{0, 0, 0, 0},
    reload_audio_counter{0, 0, 0, 0}
{
    internal_RAM.resize(0x2000, 0); 
    high_RAM.resize(0x7f, 0);
    init_registers();
}

u8 Memory::read(u16 addr) 
{
    if (enable_boot_rom && addr <= 0xff) { 
        return boot_ROM[addr];
    }
    else if (addr <= 0x7fff) {
        // Cartridge ROM banks
        return cartridge->read(addr);
    }
    else if (addr >= 0x8000 && addr <= 0x9fff) {
        // VRAM
        return gpu->read(addr);
        // return video_RAM[addr - 0x8000];
    }
    else if (addr >= 0xa000 && addr <= 0xbfff) {
        // Cartridge RAM
        return cartridge->read(addr);
    }
    else if (addr >= 0xc000 && addr <= 0xdfff) {
        // Main RAM
        return internal_RAM[addr - 0xc000];
    }
    else if (addr >= 0xe000 && addr <= 0xfdff) {
        // Echo RAM
        return read(addr - 0x2000);
    }
    else if (addr >= 0xfe00 && addr <= 0xfe9f) {
        // Sprite attribute table (OAM)
        return gpu->read(addr);
        // return sprite_attribute_table[addr - 0xfe00];
    }
    else if (addr >= 0xfea0 && addr <= 0xfeff) {
        // Unused
        return 0xff;
    }
    else if (addr >= 0xff00 && addr <= 0xff7f) {
        // IO registers
        if (addr >= 0xff10 && addr <= 0xff3f) {
            // APU registers
            return apu->read(addr);
        }
        else if (addr >= 0xff40 && addr <= 0xff4b && addr != reg::DMA) {
            // GPU registers, excluding DMA which is still handled by MMU
            return gpu->read(addr);
        }
        else {
            return read_reg(addr);
        }
    }
    else if (addr >= 0xff80 && addr <= 0xfffe) {
        return high_RAM[addr - 0xff80];
    }
    else if (addr == 0xffff) {
        return ie_reg;
    }
}

void Memory::write(u16 addr, u8 data)
{   
    if (enable_break_pt && addr == break_pt) 
        paused = true; 

    if (addr <= 0x7fff) {
        // Cartridge ROM banks
        cartridge->write(addr, data);
    }
    else if (addr >= 0x8000 && addr <= 0x9fff) {
        // VRAM
        gpu->write(addr, data);
        // video_RAM[addr - 0x8000] = data;
    }
    else if (addr >= 0xa000 && addr <= 0xbfff) {
        // Cartridge RAM
        cartridge->write(addr, data);
    }
    else if (addr >= 0xc000 && addr <= 0xdfff) {
        // Main RAM
        internal_RAM[addr - 0xc000] = data;
    }
    else if (addr >= 0xe000 && addr <= 0xfdff) {
        // Echo RAM
        write(addr - 0x2000, data);
    }
    else if (addr >= 0xfe00 && addr <= 0xfe9f) {
        // Sprite attribute table (OAM)
        gpu->write(addr, data);
        // sprite_attribute_table[addr - 0xfe00] = data;
    }
    else if (addr >= 0xfea0 && addr <= 0xfeff) {
        // Unused
        return;
    }
    else if (addr >= 0xff00 && addr <= 0xff7f) {
        // IO registers
        if (addr >= 0xff10 && addr <= 0xff3f) {
            // APU registers
            apu->write(addr, data);
        }
        else if (addr >= 0xff40 && addr <= 0xff4b && addr != reg::DMA) {
            // GPU registers, excluding DMA which is still handled by MMU
            gpu->write(addr, data);
        }
        else {
            write_reg(addr, data);
        }
    }
    else if (addr >= 0xff80 && addr <= 0xfffe) {
        high_RAM[addr - 0xff80] = data;
    }
    else if (addr == 0xffff) {
        ie_reg = data;
    }
}

u8 Memory::read_reg(u16 addr) 
{
    u8 reg_val = io_registers[addr - 0xff00] | io_read_masks[addr - 0xff00];
    switch(addr) 
    {
    case reg::IF:
        return interrupts->read();
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
    u8 mask = io_write_masks[addr - 0xff00];
    switch(addr)
    {
    case reg::IF:
        interrupts->write(data);
        break;
    case reg::DIV:
        reset_clock = true;
        break;
    case 0xff03:
        break;
    case reg::DMA: {
        dma_transfer(data);
        break;
    }
    case 0xff50:
        enable_boot_rom = false;
        break;
    default:
        io_registers[addr - 0xff00] = (data & (~mask)) | (io_registers[addr - 0xff00] & mask);
    }
}

u8& Memory::get_mem_reference(u16 addr)
{
    // plan to remove this function
    // assert(false);
    if (addr >= 0xff00 && addr <= 0xff7f) {
        return io_registers[addr - 0xff00];
    }
    else {
        assert(false);
    }
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
    // For proper timing CPU should still be running during this time, but only able to access HRAM
    u16 start_addr = src << 8;
    u16 oam_base = 0xfe00;
    std::vector<u8> transfer_data(0xa0, 0);
    for (int i = 0; i <= 0x9f; i++) {
        transfer_data[i] = read(start_addr + i);
    }
    gpu->dma_transfer(transfer_data.begin());
    // gpu->write(oam_base + i, read(start_addr + i));
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
    io[0xff03] = 0b11111111;
    io[reg::DIV] = 0;
    io[reg::TIMA] = 0;
    io[reg::TMA] = 0;
    io[reg::TAC] = 0b11111000;
    interrupts->write(0b11100000);
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

    for (int i = 0xff00; i < 0xff80; i++) {
        auto p = io.begin();
        if ((p = io.find(i)) != io.end()) {
            io_read_masks[i - 0xff00] = p->second;
        }
        else if (!(i >= 0xff30 && i <= 0xff3f)) {
            io_read_masks[i - 0xff00] = 0xff;
        }
    }

    // Read - only bits
    io_write_masks[0xff03 - 0xff00] = 0xff;
    io_write_masks[reg::STAT - 0xff00] = 0x7;
    io_write_masks[reg::NR52 - 0xff00] = 0b00001111;
}