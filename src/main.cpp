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
#include "window.h"

#undef main

int main(int argc, char *argv[])
{  
    Processor gb_cpu;
    GameWindow window(6);

    u8 gb_mem[0x10000] = { 0 };
    load_rom(gb_mem, "DMG_ROM.bin");

    gb_cpu.set_memory(gb_mem);

    for (int i = 0; i < 0x100; i++) {
        if (i != 0 && (i % 16 == 0)) {
            std::cout << std::endl;
        }
        std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)gb_mem[i] << ' ';
    }
    std::cout << std::endl;
    while (gb_cpu.step()) {}
    gb_cpu.print_register_values();
        
    while (!window.closed()) {
        int npixels = constants::screen_w * constants::screen_h;
        std::vector<float> pixels;
        for (int i = 0; i < npixels * 3; i++) {
            pixels.push_back(static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
        }
        window.draw_frame(pixels.data());
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
