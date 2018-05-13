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

#undef main

int main(int argc, char *argv[])
{  
    Processor gb_cpu;
    GPU gb_gpu;
    //std::cout << std::stoi(argv[1]) << std::endl;
    //GameWindow window(std::stoi(argv[1]));
    GameWindow window(5);

    u8 gb_mem[0x10000] = {0};
    load_rom(gb_mem, "DMG_ROM.bin");
    gb_cpu.set_memory(gb_mem);
    gb_gpu.set_memory(gb_mem);


    /*for (int i = 0; i < 0x100; i++) {
        if (i != 0 && (i % 16 == 0)) {
            std::cout << std::endl;
        }
        std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)gb_mem[i] << ' ';
    }
    std::cout << std::endl;*/
    //while (gb_cpu.step()) {}
    //gb_cpu.print_register_values();

    DEBUG::setup_dot_pattern(gb_mem);
    DEBUG::setup_gradient_tile(gb_mem);

    //DEBUG::print_tile_map(gb_mem, 0);

    /*for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 16; i++) {
            std::cout << std::hex << (int)gb_mem[GPU::TILE_DATA_0 + i - 16*j] << ' ';
        }
    }*/
    std::cout << std::endl;
    gb_mem[GPU::SCROLLY] = std::stoi(argv[1]);
    gb_gpu.build_framebuffer();
    /*while (!window.closed()) {
        
        //window.draw_frame(gb_gpu.build_framebuffer());
        gb_mem[GPU::SCROLLY] += 1;
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }*/

    return 0;
}
