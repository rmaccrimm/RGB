#include "debug.h"
#include "registers.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include "gpu.h"

// return false to quit program
bool debug::menu(Processor *cpu, int &break_pt, int &access_break_pt, bool &print_instr)
{
    while (true) {
        std::string input;
        std::cout << "> ";
        getline(std::cin, input);
        utils::to_lower(input);
        std::stringstream raw_data;

        if (input == "b") {
            break_pt = prompt("Breakpoint: ");
        }
        else if (input == "m") {
            int addr = prompt("Address: ");
            int len = prompt("Length: ");
            for (int i = addr; i < addr + len; i++) {
                if ((i % 16) == 0) {
                    if (i != addr) {
                        std::cout << std::endl;
                    }
                    std::cout << "  " << std::hex << i << ": ";
                }
                int byte = (int)cpu->memory->read(i);
                raw_data << std::setw(2) << std::setfill('0') << std::hex << byte;
                std::cout << std::setw(2) << std::setfill('0') << std::hex << byte << " ";
            }
            std::string data = raw_data.str();
            utils::to_upper(data);
            std::cout << std::endl;
            // std::cout << std::endl << data << std::endl;
        }
        else if (input == "a") {
            access_break_pt = prompt("Access break point: ");
        }
        else if (input == "r") {
            print_instr = false;
            return true;
        }
        else if (input == "q") {
            return false;
        }
        else {
            print_instr = true;
            return true;
        }
    }
}

int debug::prompt(std::string msg)
{
    int in;
    std::cout << msg;
    std::cin >> std::hex >> in;
    std::cin.ignore();
    return in;
}

void debug::print_registers(Processor *cpu)
{
    std::cout << "AF:\t"  << std::setw(4) << std::setfill('0')
              << std::hex << (int)cpu->AF.value() << "\n"
              << "BC:\t"  << std::setw(4) << std::setfill('0')
              << std::hex << (int)cpu->BC.value() << "\n"
              << "DE:\t"  << std::setw(4) << std::setfill('0')
              << std::hex << (int)cpu->DE.value() << "\n"
              << "HL:\t"  << std::setw(4) << std::setfill('0')
              << std::hex << (int)cpu->HL.value() << "\n"
              << "SP:\t"  <<  std::setw(4) << std::setfill('0')
              << std::hex << (int)cpu->SP.value() << "\n"
              << "PC:\t"  << std::setw(4) << std::setfill('0')
              << std::hex << (int)cpu->PC.value() << "\n";            
}

void debug::setup_gradient_tile(u8 *memory)
{
    u8 tile[] = {0xaf, 0x05, 0xaf, 0x05, 0xaf, 0x05, 0xaf, 0x05,
                 0xff, 0xff, 0xaa, 0xaa, 0x55, 0x55, 0x00, 0x00};
    memcpy(&memory[reg::TILE_DATA_0], tile, 16);
}

void debug::print_tile_map(u8 *memory, bool map)
{
    u16 tile_map;
    if (map) {
        tile_map = reg::TILE_MAP_1;
    }
    else {
        tile_map = reg::TILE_MAP_0;
    }
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            std::cout << std::dec << (int)(i8)memory[tile_map + 32*i + j] << ' ';
        }
        std::cout << std::endl;
    }   
}

void debug::print_boot_rom(Memory *mem)
{
    for (int i = 0; i < 0x100; i++) {
        if (i != 0 && (i % 16 == 0)) {
            std::cout << std::endl;
        }
        std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)mem->read(i) << ' ';
    }
    std::cout << std::endl;
}

void debug::print_tile_data(Processor *cpu) 
{
    debug::print_registers(cpu);
    for (int i = 0; i < 1024; i++) {
        if (i != 0 && (i % 32) == 0) {
            std::cout << std::endl;
        }
        std::cout << std::setw(2) << std::setfill('0') << std::hex 
                  << (int)cpu->memory->read(reg::TILE_MAP_0 + i) << " ";
    }
    std::cout << std::endl;
    for (int i = 0; i < 16 * 0x18; i++) {
        if (i != 0 && (i % 16) == 0) {
            std::cout << std::endl;
        }
        std::cout << std::setw(2) << std::setfill('0') << std::hex 
                  << (int)cpu->memory->read(reg::TILE_DATA_1 + i) << " ";
    }
}
