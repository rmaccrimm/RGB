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
    GameWindow window(5);

    u8 gb_mem[0x10000] = {0};
    load_rom(gb_mem, "DMG_ROM.bin");
    gb_cpu.set_memory(gb_mem);
    gb_gpu.set_memory(gb_mem);

    while (gb_cpu.step()) {}
    gb_cpu.print_register_values();

    DEBUG::setup_dot_pattern(gb_mem);
    DEBUG::setup_gradient_tile(gb_mem);

    while(!window.closed()) {
        window.draw_frame(gb_gpu.build_framebuffer());
    }
    
    return 0;
}
