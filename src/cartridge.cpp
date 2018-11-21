#include "cartridge.h"
#include "util.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <cerrno>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <map>

Cartridge::Cartridge(std::string file_name) :
    mode(0),
    enable_ram(false),
    current_rom_bank(1),
    current_ram_bank(0),
    rom_bank_size(0x4000), // 16kB
    ram_bank_size(0x2000) // 8kB
{
    utils::load_file(read_only_mem, file_name);
    read_header();
}

u8 Cartridge::read(u16 addr)
{
    if (mbc == NONE) {
        return none_read(addr);
    }
    else if (mbc == MBC1) {
        return mbc1_read(addr);
    }
    else if (mbc == MBC2) {
        return mbc2_read(addr);
    }
    else if (mbc == MBC2) {
        return mbc3_read(addr);
    }
    else if (mbc == MBC5) {
        return mbc5_read(addr);
    }
}

void Cartridge::write(u16 addr, u8 data)
{
    if (mbc == NONE) {
        none_write(addr, data); 
    }
    else if (mbc == MBC1) {
        mbc1_write(addr, data);
    }
    else if (mbc == MBC2) {
        mbc2_write(addr, data);
    }
    else if (mbc == MBC2) {
        mbc3_write(addr, data);
    }
    else if (mbc == MBC5) {
        mbc5_write(addr, data);
    }
}

u8 Cartridge::none_read(u16 addr) 
{
    if (addr <= 0x7fff) {
        return read_only_mem[addr];
    }
    else if (addr >= 0xa000 && addr <= 0xbfff) {
        return 0xff;
    }
}

void Cartridge::none_write(u16 addr, u8 data)
{
    return;
}

u8 Cartridge::mbc1_read(u16 addr) 
{
    if (addr <= 0x3fff) {
        if (mode == 0) {
            return read_only_mem[addr];
        }
        else {
            u8 bank_no = current_rom_bank & 0x60; // 2 msb
            return read_only_mem[addr + (bank_no & mask_ignore_bits) * rom_bank_size];
        }
    }
    else if (addr >= 0x4000 && addr <= 0x7fff) {
        return read_only_mem[addr + ((current_rom_bank & mask_ignore_bits)-1) * rom_bank_size];
    }
    else if (addr >= 0xa000 && addr <= 0xbfff) {
        addr -= 0xa000;
        if (!enable_ram || num_ram_banks == 0) {
            return 0xff;
        }
        if (mode == 1) {
            addr += (current_ram_bank % num_ram_banks) * ram_bank_size;
        }
        return random_access_mem[addr];
    }
    else {
        assert(false);
    }
}

void Cartridge::mbc1_write(u16 addr, u8 data)
{
    if (addr <= 0x1fff) {
        enable_ram = (data & 0xf) == 0xa;
    }
    else if (addr >= 0x2000 && addr <= 0x3fff) {
        current_rom_bank &= (7 << 5);
        current_rom_bank |= (0x1f & data);
        if ((current_rom_bank & 0x1f) == 0) {
            current_rom_bank++;
        }
    }
    else if (addr >= 0x4000 && addr <= 0x5fff) {
        current_ram_bank = data & 0x3;
        current_rom_bank &= (0x1f);
        current_rom_bank |= (data & 3) << 5;
        if ((current_rom_bank & 0x1f) == 0) {
            current_rom_bank++;
        }
    }
    else if (addr >= 0x6000 && addr <= 0x7fff) {
        mode = (bool)data;
    }
    else if (addr >= 0xa000 && addr <= 0xbfff) {
        addr -= 0xa000;
        if (!enable_ram || num_ram_banks == 0) {
            return;
        }
        else {
            if (mode == 1) {
                addr += (current_ram_bank % num_ram_banks) * ram_bank_size;
            }
            random_access_mem[addr] = data;
        }
    }
}

u8 Cartridge::mbc2_read(u16 addr)
{
    assert(false);
    return 0;
}

void Cartridge::mbc2_write(u16 addr, u8 data)
{
    assert(false);
}

u8 Cartridge::mbc3_read(u16 addr)
{
    assert(false);
    return 0;
}

void Cartridge::mbc3_write(u16 addr, u8 data)
{
    assert(false);
}

u8 Cartridge::mbc5_read(u16 addr)
{
    assert(false);
    return 0;
}

void Cartridge::mbc5_write(u16 addr, u8 data)
{
    assert(false);
}
 
void Cartridge::read_header()
{
    u16 MBC_TYPE = 0x147;
    u16 ROM_SIZE = 0x148;
    u16 RAM_SIZE = 0x149;
    u16 TITLE_START = 0x134;
    u16 TITLE_END = 0x142;
       
    std::map<u8, u8> rom_bank_opts = {{0, 2},  {1, 4}, {2, 8}, {3, 16}, {4, 32}, {5, 64}, {6, 128}, 
        {0x52, 72}, {0x53, 80}, {0x54, 96}};
    std::map<u8, u8> ram_bank_opts = {{0, 0}, {1, 1}, {2, 1}, {3, 4}, {4, 16}, {5, 8}};
    
    num_rom_banks = rom_bank_opts[read_only_mem[ROM_SIZE]];
    num_ram_banks = ram_bank_opts[read_only_mem[RAM_SIZE]];
    set_type(read_only_mem[MBC_TYPE]);
    random_access_mem.resize(num_ram_banks * ram_bank_size, 0);
    auto it = read_only_mem.begin();
    title = std::string(it + TITLE_START, it + TITLE_END + 1);

    // Determine how many bits are needed to store rom bank numbers
    mask_ignore_bits = 0x0;
    int i = 0;
    while (mask_ignore_bits < (num_rom_banks - 1)) {
        mask_ignore_bits |= (1 << i);
        i++;
    }
}

void Cartridge::set_type(u8 data)
{
    switch (data) 
    {
    case 0x00: 
    case 0x08:
    case 0x09:
        mbc = NONE;
        break;
    case 0x01: 
    case 0x02:
    case 0x03: 
        mbc = MBC1;
        break;
    case 0x05:
    case 0x06:
        mbc = MBC2;
        break;
    case 0x0f: 
    case 0x10: 
    case 0x11: 
    case 0x12: 
    case 0x13: 
        mbc = MBC3;
        break;
    case 0x19: 
    case 0x1a: 
    case 0x1b: 
    case 0x1c: 
    case 0x1d: 
    case 0x1e: 
        mbc = MBC5;
        break;
    default:
        assert(false);
    }
}