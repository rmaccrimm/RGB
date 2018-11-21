#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <string>
#include <vector>
#include "definitions.h"

typedef std::vector<u8>::iterator mem_iter;

class Cartridge
{
public:
    enum MBCType {
        NONE,
        MBC1,
        MBC2,
        MBC3,
        MBC5,
    };

    Cartridge(std::string rom_filename); 

    u8 read(u16 addr);
    void write(u16 addr, u8 data);

    MBCType mbc;
    std::string title;
    
private:
    // Specific MBC type read/write implementations
    u8 none_read(u16 addr);
    u8 mbc1_read(u16 addr);
    u8 mbc2_read(u16 addr);
    u8 mbc3_read(u16 addr);
    u8 mbc5_read(u16 addr);

    void none_write(u16 addr, u8 data);
    void mbc1_write(u16 addr, u8 data);
    void mbc2_write(u16 addr, u8 data);
    void mbc3_write(u16 addr, u8 data);
    void mbc5_write(u16 addr, u8 data);

    // Initialization functions
    void read_header();
    void set_type(u8 val);

    bool mode;
    bool enable_ram;

    const int rom_bank_size;
    const int ram_bank_size;
    int current_rom_bank;
    int current_ram_bank;
    int num_rom_banks;
    int num_ram_banks;

    std::vector<u8> read_only_mem;
    std::vector<u8> random_access_mem;

    std::vector<u8>::iterator fixed_read_only_bank;
    std::vector<u8>::iterator current_read_only_bank;
    std::vector<u8>::iterator current_random_access_bank;
    
    u8 mask_ignore_bits;
    
};

#endif