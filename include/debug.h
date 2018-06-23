#ifndef DEBUG_H
#define DEBUG_H

#include <string>
#include "definitions.h"
#include "processor.h"
#include "mmu.h"

namespace debug
{
    bool menu(Processor *cpu, int &break_pt, bool &print_instr);
    void print_registers(Processor *cpu);
    int prompt(std::string msg);
    
    void setup_stripe_pattern(u8 *memory);
    void setup_dot_pattern(u8 *memory);
    void setup_gradient_tile(u8 *memory);
    void print_tile_map(u8 *memory, bool map);
    void print_tile_data(Processor *cpu);
    void print_boot_rom(Memory *mem);
}

#endif