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
    Memory gb_mem;
    gb_mem.load_cart("Tetris.gb");
    Processor gb_cpu(&gb_mem);
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
