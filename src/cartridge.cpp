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
        return mcb1_read(addr);
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
        mcb1_write(addr, data);
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

u8 mbc1_read(u16 addr) 
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
 

void Cartridge::write(u16 addr, u8 val)
{
    switch (cartridge_type)
    {
    case MBC1:
    case MBC1_RAM:
    case MBC1_RAM_BATTERY:
        switch (addr / 0x1000)
        {
        // enable/disable RAM
        case 0x0:
        case 0x1:
            if ((val & 0xf) == 0xa) {
                enable_ram = true;
            }
            else {
                enable_ram = false;
            }
            break;
        // set 5lsb of ROM bank
        case 0x2:
        case 0x3:
        {
            current_rom_bank &= (7 << 5);
            current_rom_bank |= (0x1f & val);
            if ((current_rom_bank & 0x1f) == 0)
                current_rom_bank++;
            break;
        }
        // set RAM bank/2 msb of ROM bank
        case 0x4:
        case 0x5:
                current_ram_bank = val & 0x3;
                current_rom_bank &= (0x1f);
                current_rom_bank |= (val & 3) << 5;
                if ((current_rom_bank & 0x1f) == 0)
                    current_rom_bank++;
            break;
        // select mode
        case 0x6:
        case 0x7:
            if (val == 0)
                mode = MODE0;
            else
                mode = MODE1;
            break;
        // access RAM
        case 0xa:
        case 0xb:
        {
            addr -= 0xa000;
            if (!enable_ram || num_ram_banks == 0) {
                return;
            }
            else {
                if (mode == MODE1) {
                    addr += (current_ram_bank % num_ram_banks) * ram_bank_size;
                }
                random_access_mem[addr] = val;
            }
            break;
        }
        default:
            std::cout << enable_ram << std::endl;
            std::cout << "Addr " << std::hex << addr << " fell through write" << std::endl;
            assert(false);
        }
        break;
    default:
        break;
    }
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
    cartridge_type = get_type(read_only_mem[MBC_TYPE]);
    random_access_mem.resize(num_ram_banks * ram_bank_size, 0);
    auto it = read_only_mem.begin();
    game_title = std::string(it + TITLE_START, it + TITLE_END + 1);

    // Determine how many bits are needed to store rom bank numbers
    mask_ignore_bits = 0x0;
    int i = 0;
    while (mask_ignore_bits < (num_rom_banks - 1)) {
        mask_ignore_bits |= (1 << i);
        i++;
    }

    std::cout << "Succesfully loaded " << title() << std::endl 
              << "Cartridge type - " << type() << std::endl
              << num_rom_banks << " ROM banks" << std::endl
              << num_ram_banks << " RAM banks" << std::endl;

    assert(read_only_mem.size() == (unsigned int)(num_rom_banks * rom_bank_size));
    std::vector<CartridgeType> im = { ROM_ONLY, MBC1, MBC1_RAM, MBC1_RAM_BATTERY };
    bool mbc_type_implemented = std::find(
        im.begin(), im.end(), cartridge_type) != im.end();
    assert(mbc_type_implemented);
}

std::string Cartridge::title() { return game_title; }

std::string Cartridge::type()
{
    switch (cartridge_type) {
        case INVALID: return "INVALID";
        case ROM_ONLY: return "ROM ONLY";
        case MBC1: return "MBC1";
        case MBC1_RAM: return "MBC1+RAM";
        case MBC1_RAM_BATTERY: return "MBC1+RAM+BATTERY";
        case MBC2: return "MBC2";
        case MBC2_BATTERY: return "MBC2+BATTERY";
        case ROM_RAM: return "ROM+RAM";
        case ROM_RAM_BATTERY: return "ROM+RAM+BATTERY";
        case MMM01: return "MMM01";
        case MMM01_RAM: return "MMM01+RAM";
        case MMM01_RAM_BATTERY: return "MMM01+RAM+BATTERY";
        case MBC3_TIMER_BATTERY: return "MBC3+TIMER+BATTERY";
        case MBC3_RAM_TIMER_BATTERY: return "MBC3+RAM+TIMER+BATTERY";
        case MBC3: return "MBC3";
        case MBC3_RAM: return "MBC3+RAM";
        case MBC3_RAM_BATTERY: return "MBC3+RAM+BATTERY"; return "MBC3+RAM+BATTERY";
        case MBC5: return "MBC5";
        case MBC5_RAM: return "MBC5+RAM";
        case MBC5_RAM_BATTERY: return "MBC5+RAM+BATTERY";
        case MBC5_RUMBLE: return "MBC5+RUMBLE";
        case MBC5_RUMBLE_RAM: return "MBC5+RUMBLE+RAM";
        case MBC5_RUMBLE_RAM_BATTERY: return "MBC5+RUMBLE+RAM+BATTERY";
        case MBC6: return "MBC6";
        case MBC7_SENSOR_RUMBLE_RAM_BATTERY: return "MBC7+SENSOR+RUMBLE+RAM+BATTERY";
        case POCKET_CAMERA: return "POCKET+CAMERA";
        case BANDAI_TAMA5: return "BANDAI+TAMA5";
        case HuC3: return "HuC3";
        case HuC1_RAM_BATTERY: return "HuC1+RAM+BATTERY";
        default:
            return "UNKNOWN";
    }
}

Cartridge::CartridgeType Cartridge::get_type(u8 val)
{
    switch (val) {
        case 0x00: return Cartridge::ROM_ONLY;
        case 0x01: return Cartridge::MBC1;
        case 0x02: return Cartridge::MBC1_RAM;
        case 0x03: return Cartridge::MBC1_RAM_BATTERY;
        case 0x05: return Cartridge::MBC2;
        case 0x06: return Cartridge::MBC2_BATTERY;
        case 0x08: return Cartridge::ROM_RAM;
        case 0x09: return Cartridge::ROM_RAM_BATTERY;
        case 0x0b: return Cartridge::MMM01;
        case 0x0c: return Cartridge::MMM01_RAM;
        case 0x0d: return Cartridge::MMM01_RAM_BATTERY;
        case 0x0f: return Cartridge::MBC3_TIMER_BATTERY;
        case 0x10: return Cartridge::MBC3_RAM_TIMER_BATTERY;
        case 0x11: return Cartridge::MBC3;
        case 0x12: return Cartridge::MBC3_RAM;
        case 0x13: return Cartridge::MBC3_RAM_BATTERY;
        case 0x19: return Cartridge::MBC5;
        case 0x1a: return Cartridge::MBC5_RAM;
        case 0x1b: return Cartridge::MBC5_RAM_BATTERY;
        case 0x1c: return Cartridge::MBC5_RUMBLE;
        case 0x1d: return Cartridge::MBC5_RUMBLE_RAM;
        case 0x1e: return Cartridge::MBC5_RUMBLE_RAM_BATTERY;
        case 0x20: return Cartridge::MBC6;
        case 0x22: return Cartridge::MBC7_SENSOR_RUMBLE_RAM_BATTERY;
        case 0xfc: return Cartridge::POCKET_CAMERA;
        case 0xfd: return Cartridge::BANDAI_TAMA5;
        case 0xfe: return Cartridge::HuC3;
        case 0xff: return Cartridge::HuC1_RAM_BATTERY;
        default: return Cartridge::INVALID;
    }
}
