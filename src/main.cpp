#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL.h>

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
    GameWindow game_window;

    size_t rom_size;
    std::vector<u8> rom_data = read_rom(rom_size, "DMG_ROM.bin");
    gb_cpu.map_to_memory(rom_data.data(), rom_size, 0);
        
    while (!game_window.closed()) {
        int npixels = SCREEN_W * SCREEN_H;
        std::vector<float> pixels;
        for (int i = 0; i < npixels * 3; i++) {
            pixels.push_back(static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
        }
        game_window.draw_frame(pixels.data());
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
