#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <string>
#include <vector>
#include "definitions.h"

class Cartridge
{
public:
    enum CartridgeType {
        ROM_ONLY                        = 0x00,
        MBC1                            = 0x01,
        MBC1_RAM                        = 0x02,
        MBC1_RAM_BATTERY                = 0x03,
        MBC2                            = 0x05,
        MBC2_BATTERY                    = 0x06,
        ROM_RAM                         = 0x08,
        ROM_RAM_BATTERY                 = 0x09,
        MMM01                           = 0x0b,
        MMM01_RAM                       = 0x0c,
        MMM01_RAM_BATTERY               = 0x0d,
        MBC3_TIMER_BATTERY              = 0x0f,
        MBC3_RAM_TIMER_BATTERY          = 0x10,
        MBC3                            = 0x11,
        MBC3_RAM                        = 0x12,
        MBC3_RAM_BATTERY                = 0x13,
        MBC5                            = 0x19,
        MBC5_RAM                        = 0x1a,
        MBC5_RAM_BATTERY                = 0x1b,
        MBC5_RUMBLE                     = 0x1c,
        MBC5_RUMBLE_RAM                 = 0x1d,
        MBC5_RUMBLE_RAM_BATTERY         = 0x1e,
        MBC6                            = 0x20,
        MBC7_SENSOR_RUMBLE_RAM_BATTERY  = 0x22,
        POCKET_CAMERA                   = 0xfc,
        BANDAI_TAMA5                    = 0xfd,
        HuC3                            = 0xfe,
        HuC1_RAM_BATTERY                = 0xff
    };

    enum Mode { ROM, RAM };

    Cartridge(std::string filename); 

    u8 read(u16 addr);
    void write(u16 addr, u8 val);

private:
    CartridgeType cartridge_type;
    std::string title;
    bool mode;
    bool gbc; // color cartridge
    bool sgb; // super gameboy functions
    bool enable_ram;
    int rom_size;
    int ram_size;
    const int ram_bank_size;
    const int rom_bank_size;
    
    u8 current_rom_bank;
    u8 current_ram_bank;
    std::vector<u8> rom_data;

    std::vector<u8> read_rom(size_t &Filesize, std::string path);

};

#endif