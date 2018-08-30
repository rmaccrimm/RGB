#include "debug.h"
#include "registers.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include "gpu.h"

bool debug::menu(Processor *cpu, int &break_pt, bool &print_instr)
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
            int addr = prompt("Adress: ");
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
              << std::hex << (int)cpu->PC.value() << "\n"
              << "TAC:\t" << std::setw(4) << std::setfill('0')
              << std::hex << (int)cpu->memory->read(reg::TAC) << "\n"
              << "DIV:\t" << std::setw(4) << std::setfill('0')
              << std::hex << (int)cpu->memory->read(reg::DIV) << "\n"
              << "TIMA:\t" << std::setw(4) << std::setfill('0')
              << std::hex << (int)cpu->memory->read(reg::TIMA) << "\n";
}

void debug::setup_stripe_pattern(u8 *memory)
{
    u8 lcdc = 0;
    // enable lcd
    lcdc |= 1 << 7;
    // tile data 1, unsigned
    lcdc |= 1 << 4;
    // enable bg
    lcdc |= 1;
    
    memory[reg::LCDC] = lcdc;
    memory[reg::SCROLLX] = 0;
    memory[reg::SCROLLY] = 0;

    u8 colors[] = {0xff, 0xaa, 0x55, 0};
    for (u8 i = 0; i < 64; i++) {
        memory[GPU::TILE_DATA_1 + i] = colors[i/16];
    } 
    for (u8 i = 0; i < 32; i++) {
        for (u8 j = 0; j < 32; j++) {
            memory[GPU::TILE_MAP_0 + (32 * i) + j] = i % 4;
        }
    }
}

void debug::setup_dot_pattern(u8 *memory)
{
    u8 lcdc = 0;
    lcdc |= 1;
    lcdc |= 1 << 7;
    // tile data 0, signed
    memory[reg::LCDC] = lcdc;
    memory[reg::SCROLLX] = 0;
    memory[reg::SCROLLY] = 0;

    u8 tile0[] = {0x22, 0x22, 0x88, 0x88, 0x22, 0x22, 0x88, 0x88,
                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    u8 tile1[] = {0xdd, 0xdd, 0x77, 0x77, 0xdd, 0xdd, 0x77, 0x77, 
                  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    u8 tile2[] = {0x99, 0x99, 0x66, 0x66, 0x99, 0x99, 0x66, 0x66, 
                  0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
    u8 tile3[] = {0x33, 0x33, 0xcc, 0xcc, 0x33, 0x33, 0xcc, 0xcc,
                  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
    memcpy(&memory[GPU::TILE_DATA_0], tile0, 16);
    memcpy(&memory[GPU::TILE_DATA_0 - 16], tile1, 16);
    memcpy(&memory[GPU::TILE_DATA_0 - 32], tile2, 16);
    memcpy(&memory[GPU::TILE_DATA_0 - 48], tile3, 16);
    for (u8 i = 0; i < 32; i++) {
        for (u8 j = 0; j < 32; j++) {
            memory[GPU::TILE_MAP_0 + (32 * i) + j] = -(i % 4);
        }
    }
}

void debug::setup_gradient_tile(u8 *memory)
{
    u8 tile[] = {0xaf, 0x05, 0xaf, 0x05, 0xaf, 0x05, 0xaf, 0x05,
                 0xff, 0xff, 0xaa, 0xaa, 0x55, 0x55, 0x00, 0x00};
    memcpy(&memory[GPU::TILE_DATA_0], tile, 16);
}

void debug::print_tile_map(u8 *memory, bool map)
{
    u16 tile_map;
    if (map) {
        tile_map = GPU::TILE_MAP_1;
    }
    else {
        tile_map = GPU::TILE_MAP_0;
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
