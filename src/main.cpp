#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <Windows.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL.h>

#include "debug.h"
#include "definitions.h"
#include "register16bit.h"
#include "register8bit.h"
#include "processor.h"
#include "video.h"

#undef main

int main(int argc, char *argv[])
{  
    GameWindow game_window;
		
    while (!game_window.closed()) {
		float pixels[48];
		for (int i = 0; i < 48; i++) {
			pixels[i] = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		}
        game_window.draw_frame(pixels);
		Sleep(100);
    }
    
    /*Processor gb_cpu;
    size_t rom_size;
    std::vector<u8> rom_data = read_rom(rom_size, "DMG_ROM.bin");

    gb_cpu.map_to_memory(rom_data.data(), rom_size, 0);
    gb_cpu.run();
    
    for (unsigned int i = 0; i < rom_data.size(); i++) {
        if (i != 0 && (i % 16 == 0))
            std::cout << std::endl;
        std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)rom_data[i] << ' ';
    }
    std::cout << std::endl;
    
    gb_cpu.print_register_values();*/
    
    //SDL_DestroyWindow(window);
    

    return 0;
}
