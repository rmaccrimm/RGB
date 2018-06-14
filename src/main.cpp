#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>

#include <iostream>
#include <iomanip>

#include "debug.h"
#include "definitions.h"
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

int main(int argc, char *argv[])
{  
    Memory gb_mem;
    gb_mem.load_cart("Tetris.gb");
    Processor gb_cpu(&gb_mem);

    run_tests(&gb_cpu);

    //GPU gb_gpu(&gb_mem);
    //GameWindow window(5);    

    for (int i = 0; i < 0x150; i++) {
        if (i != 0 && (i % 16 == 0)) {
            std::cout << std::endl;
        }
        std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)gb_mem.read(i) << ' ';
    }
    std::cout << std::endl;
    gb_cpu.print_registers();

    std::cout << std::endl;
    while (gb_cpu.step(0x100)) {}
    std::cout << std::endl;

    /*while(!window.closed()) {
        window.draw_frame(gb_gpu.build_framebuffer());
    }*/

    return 0;
}
