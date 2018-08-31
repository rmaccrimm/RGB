#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <string>
#include <thread>
#include <chrono>
#include <SDL2/SDL.h>

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

int main(int argc, char *argv[])
{  
    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " [-d]";
        return -1;
    }
    bool debug = false;
    if (argc == 2) {
        std::string s(argv[1]);
        if (s == "-d") {
            debug = true;
        }
        else {
            std::cerr << "Usage: " << argv[0] << " [-d]";
            return -1;
        }
    }
    int DEBUG_MODE = debug;
    bool enable_boot_rom = false;

    Register16bit clock_counter;
    Memory gb_mem(&clock_counter, enable_boot_rom);
    Processor gb_cpu(&gb_mem, &clock_counter);
    GameWindow window(5);    
    GPU gb_gpu(&gb_mem, &window);

    if (enable_boot_rom) {
        gb_mem.load_boot("DMG_ROM.bin");
    }

    std::string rom_name;
    std::ifstream rom_file("rom_name.txt");
    std::getline(rom_file, rom_name);
    if (rom_name != "") {
        gb_mem.load_cart(rom_name.c_str(), 0);
        gb_cpu.init_state();
        if (!enable_boot_rom) {
            gb_cpu.PC.set(0x100); // Start of cartridge execution
        }
    }
    
    int break_pt = -1;
    bool step_instr = false;
    if (DEBUG_MODE) {
        debug::menu(&gb_cpu, break_pt, step_instr);
    }

    while (!window.closed()) {
        if (DEBUG_MODE) {
            if (gb_cpu.PC.value() == break_pt || step_instr) {
                debug::print_registers(&gb_cpu);
                if (!debug::menu(&gb_cpu, break_pt, step_instr)) {
                    break;
                }
            }
        }
        int cycles = gb_cpu.step(step_instr);
        gb_gpu.step(cycles);
    }
    debug::print_registers(&gb_cpu);

    return 0;
}

