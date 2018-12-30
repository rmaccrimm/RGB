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
#include <boost/program_options.hpp>

#include "apu.h"
#include "cartridge.h"
#include "debug.h"
#include "definitions.h"
#include "registers.h"
#include "processor.h"
#include "gpu.h"
#include "window.h"
#include "joypad.h"
#include "string"
#include "mmu.h"
#include "assembly.h"

#define PRINT(x) std::cout << #x": " << std::hex << std::setw(4) << std::setfill('0') << (int)x << std::endl;

#undef main

namespace po = boost::program_options;

int main(int argc, char *argv[])
{  
    po::options_description desc("Usage");
    desc.add_options()
        ("help,h", "produce help message")
        ("boot-rom,b", po::value<std::string>(), "provide boot rom")
        ("debug,d", "enable debug mode")
        ("unlock,u", "unlock framerate")
        ("scale,s", po::value<int>(), "resolution scale")
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
    bool enable_debug_mode = false;
    bool step_instr = false;
    bool unlock_framerate = false;
    int scale = 5;

    if (var_map.count("boot-rom")) {
        enable_boot_rom = true;
        boot_rom_filename = var_map["boot-rom"].as<std::string>();
    }
    if (var_map.count("debug")) {
        enable_debug_mode = true;
        step_instr = true;
    }
    if (var_map.count("unlock")) {                  
        unlock_framerate = true;
    }
    if(var_map.count("scale")) {
        scale = var_map["scale"].as<int>();
    }
    
    Joypad gb_pad;
    Cartridge game_cart(cartridge_filename);
    GameWindow window(&gb_pad, scale, game_cart.title);
    Memory gb_mem(&game_cart, &gb_pad, enable_boot_rom);
    Processor gb_cpu(&gb_mem);
    GPU gb_gpu(&gb_mem, &window);
    APU gb_apu(&gb_mem);
    gb_apu.start();

    std::cout << game_cart.title << std::endl << game_cart.type << std::endl
              << game_cart.num_ram_banks << " RAM banks" << std::endl
              << game_cart.num_rom_banks << " ROM banks" << std::endl;

    if (enable_boot_rom) {
        gb_mem.load_boot(boot_rom_filename.c_str());
    } 
    else {
        gb_cpu.init_state();
    }

    int break_pt = -1;
    int access_break_pt = -1;  

    using namespace std::chrono;

    double framerate = 60.0;
    duration<double> T(1.0 / framerate);
    duration<double> dt;
    steady_clock::time_point t = steady_clock::now();

    while (!window.closed()) {
        window.process_input();

        if (enable_debug_mode) {
            if (gb_cpu.PC.value == break_pt || step_instr || gb_mem.pause() || window.paused()) {
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
        gb_apu.step(cycles);

        if (gb_gpu.frame_drawn) {
            auto t_draw = steady_clock::now();
            dt = duration_cast<duration<double>>(t_draw - t);
            if ((dt < T) && !unlock_framerate) {
                milliseconds pause = duration_cast<milliseconds>(T - dt);
                std::this_thread::sleep_for(pause);
            } 
            t = steady_clock::now();
            gb_gpu.frame_drawn = false;
        }
    }

    if (enable_debug_mode) {
        debug::print_registers(&gb_cpu);
    }

    return 0;
}

