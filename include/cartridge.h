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

    enum Mode { ROM, RAM };

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
    CartridgeType cartridge_type;
    std::string game_title;
    bool mode;
    bool gbc; // color cartridge
    bool sgb; // super gameboy functions
    bool enable_ram;
    int cartridge_file_size;
    int rom_size;
    int ram_size;
    const int ram_bank_size;
    const int rom_bank_size;

    u8 current_rom_bank;
    u8 current_ram_bank;
    std::vector<u8> cartridge_data;

    CartridgeType get_type(u8 val);
    int get_rom_size(u8 val);
    int get_ram_size(u8 val);
};

#endif