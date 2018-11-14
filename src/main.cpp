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
#include <boost/program_options.hpp>

#include "cartridge.h"
#include "debug.h"
#include "definitions.h"
#include "registers.h"
#include "register16bit.h"
#include "register8bit.h"
#include "processor.h"
#include "gpu.h"
#include "window.h"
#include "joypad.h"
#include "string"
#include "mmu.h"
#include "assembly.h"

#undef main

namespace po = boost::program_options;

int main(int argc, char *argv[])
{  
    po::options_description desc("Usage");
    desc.add_options()
        ("help,h", "produce help message")
        ("boot-rom,b", po::value<std::string>(), "provide boot rom")
        ("debug,d", "enable debug mode")
        ("input-file", po::value<std::string>(), "rom file to load");
    po::positional_options_description p_desc;
    p_desc.add("input-file", -1);
    
    po::variables_map var_map;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p_desc).run(), var_map);

    if (var_map.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    po::notify(var_map);

    std::string cartridge_filename = var_map["input-file"].as<std::string>();
    std::string boot_rom_filename;
    bool enable_boot_rom = false;
    int enable_debug_mode = false;
    bool step_instr = false;

    if (var_map.count("boot-rom")) {
        enable_boot_rom = true;
        boot_rom_filename = var_map["boot-rom"].as<std::string>();
    }
    if (var_map.count("debug")) {
        enable_debug_mode = true;
        step_instr = true;

    }    

    Joypad gb_pad;
    Cartridge game_cart(cartridge_filename);
    GameWindow window(&gb_pad, 5, game_cart.title());    
    Memory gb_mem(&game_cart, &gb_pad, enable_boot_rom);
    Processor gb_cpu(&gb_mem);
    GPU gb_gpu(&gb_mem, &window);

    if (enable_boot_rom) {
        gb_mem.load_boot(boot_rom_filename.c_str());
    } 
    else {
        gb_cpu.init_state();
    }

    int break_pt = -1;
    int access_break_pt = -1;
    
    while (!window.closed()) {
        window.process_input();
        if (enable_debug_mode) {
            if (gb_cpu.PC.value() == break_pt || step_instr || gb_mem.pause() || window.paused()) {
                debug::print_registers(&gb_cpu);
                if (!debug::menu(&gb_cpu, break_pt, access_break_pt, step_instr)) {
                    break;
                }
                if (access_break_pt >= 0) {
                    gb_mem.set_access_break_pt(access_break_pt);
                }
            }
        }
        int cycles = gb_cpu.step(step_instr);
        gb_gpu.step(cycles);
		if (window.frame_drawn()) {
			if (window.closed()) {
				break;
			}
			window.process_input();
		}
    }
    if (enable_debug_mode) {
        debug::print_registers(&gb_cpu);
    }

    return 0;
}

