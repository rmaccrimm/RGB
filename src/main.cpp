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
#include "tests.h"
#include "include/memory.h"
#include "assembly.h"
#undef main

void run_test_rom(Processor *cpu, std::vector<u8> &rom, std::string name)
{
    cpu->memory->load(rom.data(), 0, rom.size());
    while(cpu->step(rom.size())) {}
    std::cout << std::endl << "RESULT: " << name << std::endl;
    cpu->print_registers();
    std::cout << std::endl;
    cpu->PC.set(0);
}

void run_tests(Processor *cpu)
{
    run_test_rom(cpu, TEST::ld_immediate_8bit, "ld_immediate_8bit");
    run_test_rom(cpu, TEST::ld_immediate_16bit, "ld_immediate_16bit");
    run_test_rom(cpu, TEST::ld_register_8bit, "ld_register_8bit");
    run_test_rom(cpu, TEST::ld_address, "ld_address");
    run_test_rom(cpu, TEST::stack, "stack");
}

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

int main(int argc, char *argv[])
{  
    GameWindow window(5);    
    Memory gb_mem;
    Processor gb_cpu(&gb_mem);
    GPU gb_gpu(&gb_mem, &window);
    gb_mem.load_cart("Tetris.gb");

    //run_tests(&gb_cpu);

    while (!window.closed()) {
        int cycles = gb_cpu.step(0x100);
        if (cycles < 0) {
            break;
        }
        gb_gpu.step(cycles);
    }
    gb_cpu.print_registers();
    for (int i = 0; i < 256; i++) {
        if (i != 0 && (i % 16) == 0) {
            std::cout << std::endl;
        }
        std::cout << (int)gb_mem.read(GPU::TILE_MAP_0 + i) << " ";
    }

    return 0;
}
