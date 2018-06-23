#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <string>

#include "debug.h"
#include "definitions.h"
#include "registers.h"
#include "register16bit.h"
#include "register8bit.h"
#include "processor.h"
#include "gpu.h"
#include "window.h"
#include "string"
#include "mmu.h"
#include "assembly.h"
#undef main

void print_boot_rom(Memory *mem)
{
    for (int i = 0; i < 0x100; i++) {
        if (i != 0 && (i % 16 == 0)) {
            std::cout << std::endl;
        }
        std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)mem->read(i) << ' ';
    }
    std::cout << std::endl;
}

void print_tile_data(Processor *cpu) 
{
    cpu->print_registers();
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
        std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)cpu->memory->read(reg::TILE_DATA_1 + i) << " ";
    }
}

void to_lower(std::string &s) 
{
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
} 

int main(int argc, char *argv[])
{  
    Memory gb_mem;
    Processor gb_cpu(&gb_mem);
    GameWindow window(5);    
    GPU gb_gpu(&gb_mem, &window);
    // gb_mem.load_cart("Tetris.gb");
    //gb_mem.load_cart("cpu_instrs.gb", 0);
    gb_cpu.init_state();
    //gb_cpu.PC.set(0x100);

    bool quit = false;
    int break_pt = -1;
    if (DEBUG_MODE) {
        std::cout << "Break point: ";
        std::cin >> std::hex >> break_pt;
    }

    while (!window.closed()) {
        int cycles = gb_cpu.step(break_pt, false);
        gb_gpu.step(cycles);

        if (gb_cpu.PC.value() == break_pt) {
            gb_cpu.print_registers();
            // debug::menu(&gb_cpu);
            std::string s;
            bool run = false;    
            while (!run) {
                getline(std::cin, s);
                to_lower(s);
                if (s == "m") {
                    u16 addr, len;
                    std::cout << "address: ";
                    std::cin >> std::hex >> addr;
                    std::cout << "length: ";
                    std::cin >> std::hex >> len;

                    for (int i = addr; i < addr + len; i++) {
                        if ((i % 16) == 0) {
                            std::cout << std::endl << std::hex << i << ": ";
                        }
                        std::cout << std::setw(2) << std::setfill('0') << std::hex 
                                << (int)gb_mem.read(i) << " ";
                    }
                    std::cout << std::endl << std::endl;
                } 
                else if (s == "b") {
                    std::cout << "Break point: ";
                    std::cin >> std::hex >> break_pt;
                }
                else if (s == "s") {
                    cycles = gb_cpu.step(-1, true);
                    gb_gpu.step(cycles);
                    gb_cpu.print_registers();
                } 
                else if (s == "q") {
                    quit = true;
                    run = true; 
                } 
                else {
                    run = true;
                }
            }
        }
        if (quit) {
            break;
        }
    }
    return 0;
}

