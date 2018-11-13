#include "cartridge.h"
#include "functions.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <cerrno>
#include <cassert>
#include <cstring>
#include <algorithm>

Cartridge::Cartridge(std::string file_name) : 
    mode(ROM), current_rom_bank(1), current_ram_bank(0), 
    rom_bank_size(0x4000), // 16kB 
    ram_bank_size(0x2000), // 8kB 
    enable_ram(false)
{
    utils::load_file(cartridge_data, file_name);
    cartridge_file_size = cartridge_data.size();
    cartridge_type = get_type(cartridge_data[0x147]);
    auto it = cartridge_data.begin();
    game_title = std::string(it + 0x134, it + 0x143);

    std::vector<CartridgeType> implemented = { ROM_ONLY, MBC1, MBC1_RAM, MBC1_RAM_BATTERY };
    assert(std::find(implemented.begin(), implemented.end(), cartridge_type) != implemented.end());
}

u8 Cartridge::read(u16 addr) 
{
    switch (cartridge_type) 
    {
    case ROM_ONLY:
        return cartridge_data[addr];
        break;

    case MBC1:
    case MBC1_RAM:
    case MBC1_RAM_BATTERY:
        if (addr <= 0x3fff) {
            return cartridge_data[addr];
        }
        else if ((addr >= 0x4000) && (addr <= 0x7fff)) {
            u16 rom_bank_size = 0x4000; // 4kb
            return cartridge_data[addr + ((current_rom_bank - 1) * rom_bank_size)];
        }
        else if ((addr >= 0xa000) && (addr <= 0xbfff)) {
            if (!enable_ram) {
                return 0;
            }
            else {
                return cartridge_data[addr + current_ram_bank * ram_bank_size];
            }
        }
        break;
    }
    std::cout << enable_ram << std::endl;
    std::cout << "Addr " << std::hex << addr << " fell through read" << std::endl;
    assert(false);        
}

void Cartridge::write(u16 addr, u8 val)
{
    switch (cartridge_type)
    {
    case MBC1:
    case MBC1_RAM:
    case MBC1_RAM_BATTERY:
        if (addr <= 0x1fff) { // enable/disable RAM
            if ((val & 0xf) == 0xa) {
                enable_ram = true;
            }
            else {
                enable_ram = false;
            }
        }
        if ((addr >= 0x2000) && (addr <= 0x3fff)) { // set 5lsb of ROM bank
            current_rom_bank &= (7 << 5); 
            current_rom_bank |= (0x1f & val); 
            if ((current_rom_bank & 0x1f) == 0) 
                current_rom_bank++;
        }
        else if ((addr >= 0x4000) && (addr <= 0x5fff)) { // set RAM bank/2 msb of ROM bank 
            if (mode == RAM) {
                if (!enable_ram) { // ? 
                    return;
                }
                current_ram_bank = val & 0x3;
            }
            else if (mode == ROM) {
                current_rom_bank &= (0x1f); 
                current_rom_bank |= (val & 3) << 5; 
                if ((current_rom_bank & 0x1f) == 0)
                    current_rom_bank++;
            }
        }
        else if ((addr >= 0x6000) && (addr <= 0x7fff)) { // select mode
            if (val == 0)
                mode = ROM;
            else
                mode = RAM;
        }
        else if ((addr >= 0xa000) && (addr <= 0xbfff)) { // access RAM
            if (!enable_ram) {
                return;
            }
            else {
                cartridge_data[addr + current_ram_bank * ram_bank_size] = val;
            }
        }
        else {
            std::cout << enable_ram << std::endl;
            std::cout << "Addr " << std::hex << addr << " fell through write" << std::endl;
            assert(false);        
        }
        break;
    }


}

std::string Cartridge::title() { return game_title; }

std::string Cartridge::type() 
{
    switch (cartridge_type) {
        case INVALID: return "INVALID";
        case ROM_ONLY: return "ROM_ONLY";
        case MBC1: return "MBC1";       
        case MBC1_RAM: return "MBC1_RAM";       
        case MBC1_RAM_BATTERY: return "MBC1_RAM_BATTERY";     
        case MBC2:                            
        case MBC2_BATTERY:                    
        case ROM_RAM:                         
        case ROM_RAM_BATTERY:                 
        case MMM01:                           
        case MMM01_RAM:                       
        case MMM01_RAM_BATTERY:               
        case MBC3_TIMER_BATTERY:              
        case MBC3_RAM_TIMER_BATTERY:          
        case MBC3:                            
        case MBC3_RAM:                        
        case MBC3_RAM_BATTERY:                
        case MBC5:                            
        case MBC5_RAM:                        
        case MBC5_RAM_BATTERY:                
        case MBC5_RUMBLE:                     
        case MBC5_RUMBLE_RAM:                 
        case MBC5_RUMBLE_RAM_BATTERY:         
        case MBC6:                            
        case MBC7_SENSOR_RUMBLE_RAM_BATTERY:  
        case POCKET_CAMERA:                   
        case BANDAI_TAMA5:                    
        case HuC3:                            
        case HuC1_RAM_BATTERY:
        default:
            return "UNIMPLEMENTED";
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