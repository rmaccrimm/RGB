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

#undef main

int main(int argc, char *argv[])
{  
    Memory gb_mem;
    Processor gb_cpu(&gb_mem);
    GPU gb_gpu(&gb_mem);
    
    //load_rom(gb_mem, "DMG_ROM.bin");
    //utils::load_rom(gb_mem, "cpu_instrs.gb");
    //load_rom(gb_mem, "Tetris.gb");

    /*load_rom(gb_mem, TEST::ld_immediate_8bit.data(), TEST::ld_immediate_8bit.size());
    while (gb_cpu.step(0)) {}
    gb_cpu.print_register_values();

    std::cout << std::endl;

    load_rom(gb_mem, TEST::ld_immediate_16bit.data(), TEST::ld_immediate_16bit.size());
    while (gb_cpu.step(0)) {}
    gb_cpu.print_register_values();

    std::cout << std::endl;

    load_rom(gb_mem, TEST::ld_register_8bit.data(), TEST::ld_register_8bit.size());
    while (gb_cpu.step(0)) {}
    gb_cpu.print_register_values();

    std::cout << std::endl;

    load_rom(gb_mem, TEST::ld_address.data(), TEST::ld_address.size());
    while (gb_cpu.step(0)) {}
    gb_cpu.print_register_values();
    std::cout << "(0xffff): " << std::hex << (int)gb_mem[0xffff] << std::endl;

    std::cout << std::endl;

    load_rom(gb_mem, TEST::stack.data(), TEST::stack.size());
    while (gb_cpu.step(0)) {}
    gb_cpu.print_register_values();*/

    
    /*for (int i = 0; i < 0x1000; i++) {
        if (i != 0 && (i % 16 == 0)) {
            std::cout << std::endl;
        }
        std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)gb_mem.read(i) << ' ';
    }
    std::cout << std::endl;
    while (gb_cpu.step(0x1000)) {
        //std::string s;
        //std::getline(std::cin, s);
    }
    std::cout << std::endl;*/

    

    /*DEBUG::setup_dot_pattern(gb_mem);
    DEBUG::setup_gradient_tile(gb_mem);*/

    GameWindow window(5);

    while(!window.closed()) {
        window.draw_frame(gb_gpu.build_framebuffer());
    }
    
    return 0;
}
