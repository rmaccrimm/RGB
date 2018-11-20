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

    Cartridge(std::string filename); 

    u8 read(u16 addr);

    void write(u16 addr, u8 data);

    void read_header();

    u8 none_read();
    u8 mbc1_read();
    u8 mbc2_read();
    u8 mbc3_read();
    u8 mbc5_read();
    
    void none_write();
    void mbc1_write();
    void mbc2_write();
    void mbc3_write();
    void mbc5_write();

    MBCType mbc;
    std::string title;
    bool mode;
    bool enable_ram;

    const int rom_bank_size;
    const int ram_bank_size;
    int current_rom_bank_index;
    int current_ram_bank_index;

    int num_rom_banks;
    int num_ram_banks;

    std::vector<u8> read_only_mem;
    std::vector<u8> random_access_mem;

    std::vector<u8>::iterator fixed_read_only_bank;
    std::vector<u8>::iterator current_read_only_bank;
    std::vector<u8>::iterator current_random_access_bank;
    
    u8 mask_ignore_bits;
    

    
    
    

    MBCType get_type(u8 val);
    
};

#endif