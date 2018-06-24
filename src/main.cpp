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

    Memory gb_mem;
    Processor gb_cpu(&gb_mem);
    GameWindow window(5);    
    GPU gb_gpu(&gb_mem, &window);
    //gb_mem.load_cart("Tetris.gb", 0x100, 0x100);
    gb_cpu.init_state();
    gb_mem.load_cart("cpu_instrs.gb", 0);
    gb_cpu.PC.set(0x100);
    
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
        // When stepping through a single instruction, print
        int cycles = gb_cpu.step(step_instr);
        gb_gpu.step(cycles);
    }

    return 0;
}

