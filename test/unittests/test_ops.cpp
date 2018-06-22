#include "test_roms.h"
#include "catch.hpp"
#include "processor.h"
#include <iostream>

void run_test_rom(Processor *cpu, std::vector<u8> &rom)
{
    cpu->memory->load(rom.data(), 0, rom.size());
    while(cpu->step(rom.size()) != -1) {}
    cpu->PC.set(0);
}

TEST_CASE("Load immediate for 8-bit registers", "[ld_immediate_8bit]") 
{

}

TEST_CASE("Load immediate for 16-bit registers", "[ld_immediate_16bit]")
{

}

TEST_CASE("Load from 8-bit register", "[ld_register_8bit]")
{
    
}

TEST_CASE("Load to/from address in 16-bit register" "[ld_address]")
{

}

TEST_CASE("Stack operations", "[stack]")
{
    Memory mem;
    Processor cpu(&mem);
    
    run_test_rom(&cpu, test::push_pop);
    REQUIRE(cpu.AF.value() == 0x14f0);
    REQUIRE(cpu.DE.value() == 0x0e34);
    REQUIRE(cpu.BC.value() == 0xaabb);
    REQUIRE(cpu.SP.value() == 0xaabb);
}

TEST_CASE("Shift operations", "[shift_ops]")
{
    Memory mem;
    Processor cpu(&mem);
    
    run_test_rom(&cpu, test::shift_left);
    REQUIRE(cpu.AF.value() == 0x7c00);

    run_test_rom(&cpu, test::shift_left_mem);
    REQUIRE(cpu.AF.value() == 0x7c10);

    run_test_rom(&cpu, test::shift_right_arithmetic);
    REQUIRE(cpu.AF.value() == 0xdf00);

    run_test_rom(&cpu, test::shift_right_arithmetic_mem);
    REQUIRE(cpu.AF.value() == 0x0a00);

    run_test_rom(&cpu, test::shift_right_logical);
    REQUIRE(cpu.AF.value() == 0x5f00);

    run_test_rom(&cpu, test::shift_right_logical_mem);
    REQUIRE(cpu.AF.value() == 0x0090);
}

TEST_CASE("Bit Operations", "[bit_ops]")
{
    Memory mem;
    Processor cpu(&mem);

    run_test_rom(&cpu, test::bit_set);
    REQUIRE(cpu.AF.value() == 0x2030);

    run_test_rom(&cpu, test::bit_reset);
    REQUIRE(cpu.AF.value() == 0xfea0);

    run_test_rom(&cpu, test::bit_mem_set);
    REQUIRE(cpu.AF.value() == 0x1530);

    run_test_rom(&cpu, test::bit_mem_reset);
    REQUIRE(cpu.AF.value() == 0x15a0);
}

TEST_CASE("Jumps", "[jumps]")
{
    Memory mem;
    Processor cpu(&mem);

    cpu.PC.set(0);
    auto rom = test::jp;
    cpu.memory->load(rom.data(), 0, rom.size());
    cpu.step();
    REQUIRE(cpu.PC.value() == 0x2fc1);

    cpu.PC.set(0);
    rom = test::jp_reg;
    cpu.memory->load(rom.data(), 0, rom.size());
    cpu.step();
    cpu.step();
    REQUIRE(cpu.PC.value() == 0xffff);

    cpu.PC.set(0);
    rom = test::jr_negative;
    cpu.memory->load(rom.data(), 0, rom.size());
    for (int i = 0; i < 9; i++) 
        cpu.step();
    REQUIRE(cpu.PC.value() == 0x0006);

    cpu.PC.set(0);
    rom = test::jr_positive;
    cpu.memory->load(rom.data(), 0, rom.size());
    for (int i = 0; i < 5; i++) 
        cpu.step();
    REQUIRE(cpu.PC.value() == 0x0016);

    cpu.PC.set(0);
    rom = test::jr_zero;
    cpu.memory->load(rom.data(), 0, rom.size());
    for (int i = 0; i < 3; i++) 
        cpu.step();
    REQUIRE(cpu.PC.value() == 0x04);
}

