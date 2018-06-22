#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>

#include <iostream>
#include <iomanip>

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


int main(int argc, char *argv[])
{  
    
    Memory gb_mem;
    Processor gb_cpu(&gb_mem);
    GameWindow window(5);    
    
    
    // //gb_cpu.PC.set(0x100);

    GPU gb_gpu(&gb_mem, &window);
    // gb_mem.load_cart("Tetris.gb");
    gb_mem.load_cart("cpu_instrs.gb", 0);
    gb_cpu.init_state();
    gb_cpu.PC.set(0x100);

    while (!window.closed()) {
        int cycles = gb_cpu.step();
        if (DEBUG_MODE) {
            /*gb_cpu.print_registers();
            std::cout << std::endl;*/
        }
        
        if (cycles < 0) {
            break;
        }
        gb_gpu.step(cycles);
    }
    return 0;
}

