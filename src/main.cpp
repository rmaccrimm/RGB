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

int main(int argc, char *argv[])
{  
    for (int i = 0; i < 256; i++) 
        std::cout << i << " " << cb_instr_set[i] << std::endl;

    /*
    Memory gb_mem;
    Processor gb_cpu(&gb_mem);
    //GPU gb_gpu(&gb_mem);
    
    for (int i = 0; i < 0x100; i++) {
        if (i != 0 && (i % 16 == 0)) {
            std::cout << std::endl;
        }
        std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)gb_mem.read(i) << ' ';
    }
    std::cout << std::endl;
    while (gb_cpu.step(0x100)) {
        //std::string s;
        //std::getline(std::cin, s);
    }
    std::cout << std::endl;*/

    

    /*DEBUG::setup_dot_pattern(gb_mem);
    DEBUG::setup_gradient_tile(gb_mem);*/

    /*GameWindow window(5);

    while(!window.closed()) {
        window.draw_frame(gb_gpu.build_framebuffer());
    }*/
    
    return 0;
}
