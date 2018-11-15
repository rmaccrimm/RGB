#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <string>
#include <vector>
#include "definitions.h"

class Cartridge
{
public:
    Cartridge(std::string filename); 

    u8 read(u16 addr);
    void write(u16 addr, u8 val);
    std::string type();
    std::string title();

    enum CartridgeType {
        INVALID,                
        ROM_ONLY,                        
        MBC1,                            
        MBC1_RAM,                        
        MBC1_RAM_BATTERY,                
        MBC2,                            
        MBC2_BATTERY,                    
        ROM_RAM,                         
        ROM_RAM_BATTERY,                 
        MMM01,                           
        MMM01_RAM,                       
        MMM01_RAM_BATTERY,               
        MBC3_TIMER_BATTERY,              
        MBC3_RAM_TIMER_BATTERY,          
        MBC3,                            
        MBC3_RAM,                        
        MBC3_RAM_BATTERY,                
        MBC5,                            
        MBC5_RAM,                        
        MBC5_RAM_BATTERY,                
        MBC5_RUMBLE,                     
        MBC5_RUMBLE_RAM,                 
        MBC5_RUMBLE_RAM_BATTERY,         
        MBC6,                            
        MBC7_SENSOR_RUMBLE_RAM_BATTERY,  
        POCKET_CAMERA,                   
        BANDAI_TAMA5,                    
        HuC3,                            
        HuC1_RAM_BATTERY
    };

private:
    enum Mode { MODE0, MODE1 };

    void read_header();

    CartridgeType cartridge_type;
    std::string game_title;
    bool mode;

    bool enable_ram;
    u8 current_rom_bank;
    u8 current_ram_bank;
    const int rom_bank_size;
    const int ram_bank_size;
    int num_rom_banks;
    int num_ram_banks;

    u8 mask_ignore_bits;
    
    std::vector<u8> read_only_mem;
    std::vector<u8> random_access_mem;

    CartridgeType get_type(u8 val);
    
};

#endif