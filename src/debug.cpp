#include "debug.h"
#include "registers.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include "gpu.h"

#define EXTRA 1

// return false to quit program
bool debug::menu(Processor *cpu, int &break_pt, int &access_break_pt, bool &print_instr)
{
    while (true) {
        std::string input;
        std::cout << "> ";
        getline(std::cin, input);
        utils::to_lower(input);
        std::stringstream raw_data;

        if (input == "b") {
            break_pt = prompt("Breakpoint: ");
        }
        else if (input == "m") {
            int addr = prompt("Address: ");
            int len = prompt("Length: ");
            for (int i = addr; i < addr + len; i++) {
                if ((i % 16) == 0) {
                    if (i != addr) {
                        std::cout << std::endl;
                    }
                    std::cout << "  " << std::hex << i << ": ";
                }
                int byte = (int)cpu->memory->read(i);
                raw_data << std::setw(2) << std::setfill('0') << std::hex << byte;
                std::cout << std::setw(2) << std::setfill('0') << std::hex << byte << " ";
            }
            std::string data = raw_data.str();
            utils::to_upper(data);
            std::cout << std::endl;
            // std::cout << std::endl << data << std::endl;
        }
        else if (input == "a") {
            access_break_pt = prompt("Access break point: ");
        }
        else if (input == "r") {
            print_instr = false;
            return true;
        }
        else if (input == "q") {
            return false;
        }
        else {
            print_instr = true;
            return true;
        }
    }
}

int debug::prompt(std::string msg)
{
    int in;
    std::cout << msg;
    std::cin >> std::hex >> in;
    std::cin.ignore();
    return in;
}

void debug::print_registers(Processor *cpu)
{
    std::cout << "AF:\t"  << std::setw(4) << std::setfill('0')
              << std::hex << (int)cpu->AF.value << "\n"
              << "BC:\t"  << std::setw(4) << std::setfill('0')
              << std::hex << (int)cpu->BC.value << "\n"
              << "DE:\t"  << std::setw(4) << std::setfill('0')
              << std::hex << (int)cpu->DE.value << "\n"
              << "HL:\t"  << std::setw(4) << std::setfill('0')
              << std::hex << (int)cpu->HL.value << "\n"
              << "SP:\t"  <<  std::setw(4) << std::setfill('0')
              << std::hex << (int)cpu->SP.value << "\n"
              << "PC:\t"  << std::setw(4) << std::setfill('0')
              << std::hex << (int)cpu->PC.value << "\n";    
    if (EXTRA) {
        std::cout << "\nTIMA:\t" << (int)cpu->memory->read(reg::TIMA) << "\n"
                  << "DIV:\t" << (int)cpu->memory->read(reg::DIV) << "\n"
                  << "TAC:\t" << (int)cpu->memory->read(reg::TAC) << "\n"
                  << "TMA:\t" << (int)cpu->memory->read(reg::TMA) << "\n";
    }
}

void debug::print_boot_rom(Memory *mem)
{
    for (int i = 0; i < 0x100; i++) {
        if (i != 0 && (i % 16 == 0)) {
            std::cout << std::endl;
        }
        std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)mem->read(i) << ' ';
    }
    std::cout << std::endl;
}