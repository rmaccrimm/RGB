#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <iostream>
#include <iomanip>
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

    Memory gb_mem;
    Processor gb_cpu(&gb_mem);
    GameWindow window(5);    
    GPU gb_gpu(&gb_mem, &window);
    gb_mem.load_cart("Dr. Mario.gb", 0);
    // gb_mem.load_cart("Tetris.gb", 0);

    // gb_mem.load_cart("01-special.gb", 0); // - PASSED
    // gb_mem.load_cart("02-interrupts.gb", 0);
    // gb_mem.load_cart("03-op sp,hl.gb", 0); // - PASSED
    // gb_mem.load_cart("04-op r,imm.gb", 0); // - PASSED
    // gb_mem.load_cart("05-op rp.gb", 0); // - PASSED
    // gb_mem.load_cart("06-ld r,r.gb", 0); // - PASSED
    // gb_mem.load_cart("07-jr,jp,call,ret,rst.gb", 0); // - PASSED
    // gb_mem.load_cart("08-misc instrs.gb", 0); // - PASSED
    // gb_mem.load_cart("09-op r,r.gb", 0); // - PASSED
    // gb_mem.load_cart("10-bit ops.gb", 0); // - PASSED
    // gb_mem.load_cart("11-op a,(hl).gb", 0); // - PASSED

    gb_cpu.init_state();
    gb_cpu.PC.set(0x100);
    
    int break_pt = -1;
    bool step_instr = false;
    if (DEBUG_MODE) {
        debug::menu(&gb_cpu, break_pt, step_instr);
    }

    while (!window.closed()) {
        if (DEBUG_MODE) {
            bool pause = false;
            bool quit = false;
            SDL_Event event;
            SDL_PollEvent(&event);
            switch (event.type) 
            {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) 
                {
                case SDLK_ESCAPE:
                    quit = true;

                    break;
                case SDLK_KP_ENTER:
                    pause = true;
                    break;
                }
            
            }
            if (quit) {
                std::cout << "HERE\n";  
                break;
            }
            if (gb_cpu.PC.value() == break_pt || step_instr || pause) {
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
    debug::print_registers(&gb_cpu);

    return 0;
}

