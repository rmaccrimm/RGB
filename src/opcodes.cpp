#include "processor.h"
#include <cassert>
#include <iostream>

//Private functions
#pragma region


void Processor::stack_push(u8 data)
{
    SP.decrement();
    memory[SP.value()] = data;
}

u8 Processor::stack_pop()
{
    u8 data = memory[SP.value()];
    SP.increment();
    return data;
}

void Processor::set_add_flags(u16 a, u16 b)
{
    set_cond(flags.carry, full_carry_add(a, b));
    set_cond(flags.half_carry, half_carry_add(a, b));
    set_cond(flags.zero, (a + b) == 0);
    reset(flags.subtract);
}

void Processor::set_sub_flags(u16 a, u16 b)
{
    set_cond(flags.carry, full_carry_sub(a, b));
    set_cond(flags.half_carry, half_carry_sub(a, b));
    set_cond(flags.zero, (a - b) == 0);
    set(flags.subtract);
}

void Processor::flag_reset(Register8bit const &reg)
{
    set_cond(flags.zero, reg.value() == 0);
    reset(flags.subtract);
    reset(flags.half_carry);
}

void Processor::flag_reset(Register16bit const &reg)
{
    set_cond(flags.zero, memory[reg.value()] == 0);
    reset(flags.subtract);
    reset(flags.half_carry);
}

void Processor::set_and_flags(u8 val)
{
    if (val == 0) {
        set(flags.zero);
    }
    reset(flags.subtract);
    set(flags.half_carry);
    reset(flags.carry);
}

void Processor::set_or_flags(u8 val)
{
    if (val == 0) {
        set(flags.zero);
    }
    reset(flags.subtract);
    reset(flags.half_carry);
    reset(flags.carry);
}

#pragma endregion


// Load operations
#pragma region

// void Processor::LD_immediate(Register8bit &reg)
// {
//     reg.set(fetch_byte());
// }

// void Processor::LD_immediate(Register16bit &reg)
// {
//     u8 l = fetch_byte(); 
//     u8 h = fetch_byte();
//     reg.set_low(l);
//     reg.set_high(h);
// }

// void Processor::LD_address(Register8bit &dest, Register16bit const &src)
// {
//     dest.set(memory[src.value()]);
// }

// void Processor::LD_address(Register16bit const &dest, Register8bit const &src)
// {
//     memory[dest.value()] = src.value();
// }

// void Processor::LD_register(Register8bit &dest, Register8bit const &src)
// {
//     dest.set(src.value());
// }

// void Processor::LD_register(Register16bit &dest, Register16bit const &src)
// {
//     dest.set(src.value());
// }

// void Processor::PUSH_register(Register8bit const &reg)
// {
//     stack_push(reg.value());
// }

// void Processor::PUSH_register(Register16bit const &reg)
// {
//     stack_push(reg.value_high());
//     stack_push(reg.value_low());
// }

// void Processor::POP_register(Register8bit &reg)
// {
//     reg.set(stack_pop());
// }

// void Processor::POP_register(Register16bit &reg)
// {
//     reg.set_low(stack_pop());
//     reg.set_high(stack_pop());
// }

#pragma endregion


// Add/Subtract operations
#pragma region

void Processor::INC_register(Register8bit &reg) 
{
    set_cond(flags.half_carry, half_carry_add(reg.value(), 1));
    reset(flags.subtract);
    reg.increment();
    set_cond(flags.zero, (reg.value() == 0));
}

void Processor::INC_register(Register16bit &reg)
{
    reg.increment();
}

void Processor::INC_address(Register16bit const &reg)
{	
    set_cond(flags.half_carry, half_carry_add(memory[reg.value()], 1));
    reset(flags.subtract);
    memory[reg.value()]++;
    set_cond(flags.zero, (memory[reg.value()] == 0));
}

void Processor::DEC_register(Register8bit &reg)
{
    set_cond(flags.half_carry, half_carry_sub(reg.value(), 1));
    set(flags.subtract);
    reg.decrement();
    set_cond(flags.zero, (reg.value() == 0));
}

void Processor::DEC_register(Register16bit &reg)
{
    reg.decrement();
}

void Processor::DEC_address(Register16bit const &reg)
{
    set_cond(flags.half_carry, half_carry_sub(memory[reg.value()], 1));
    set(flags.subtract);
    memory[reg.value()]--;
    set_cond(flags.zero, (memory[reg.value()] == 0));
}

void Processor::ADD_register(Register8bit &dest, Register8bit const &src, bool carry)
{
    int add = src.value();
    if (carry) {
        add += flags.carry;
    }
    set_add_flags(dest.value(), add);
    dest.set(dest.value() + add);
}

void Processor::ADD_register(Register16bit &dest, Register16bit const &src)
{
    u16 a = dest.value();
    u16 b = src.value();
    // carry from bit 11
    set_cond(flags.half_carry, (((a & 0xfff) + (b & 0xfff)) & 0x1000) == 0x1000);
    // carry from bit 15
    set_cond(flags.half_carry, (((a & 0xffff) + (b & 0xffff)) & 0x10000) == 0x10000);
    dest.set(dest.value() + src.value());
}

void Processor::ADD_immediate(Register8bit &reg, bool carry)
{
    int add = fetch_byte();
    if (carry) {
        add += flags.carry;
    }
    set_add_flags(reg.value(), add);
    reg.set(reg.value() + add);
}

void Processor::ADD_immediate(Register16bit &reg)
{
    i8 val = fetch_byte();
    set_cond(flags.carry, full_carry_add(reg.value_low(), val));
    set_cond(flags.half_carry, half_carry_add(reg.value_low(), val));
    reset(flags.zero);
    reset(flags.subtract);
    reg.set(reg.value() + val);
}

void Processor::ADD_address(Register8bit &dest, Register16bit const &src, bool carry)
{
    int add = memory[src.value()];
    if (carry) {
        add += flags.carry;
    }
    set_add_flags(dest.value(), add);
    dest.set(dest.value() + add);
}

void Processor::SUB_register(Register8bit &dest, Register8bit const &src, bool carry)
{
    int sub = src.value();
    if (carry) {
        sub += flags.carry;
    }
    set_add_flags(dest.value(), sub);
    dest.set(dest.value() + sub);
}

void Processor::SUB_immediate(Register8bit &reg, bool carry)
{
    int sub = fetch_byte();
    if (carry) {
        sub += flags.carry;
    }
    set_add_flags(reg.value(), sub);
    reg.set(reg.value() + sub);
}

void Processor::SUB_address(Register8bit &dest, Register16bit const &src, bool carry)
{
    int sub = memory[src.value()];
    if (carry) {
        sub += flags.carry;
    }
    set_add_flags(dest.value(), sub);
    dest.set(dest.value() + sub);
}

#pragma endregion


// Rotates
#pragma region

// void Processor::RL(Register8bit &reg)
// {
//     int temp = reg.value() >> 7; // bit 7
//     reg.set((reg.value() << 1) | (u8)flags.carry); // rotate through carry
//     set_cond(flags.carry, temp);
//     flag_reset(reg);
// }

void Processor::RR(Register8bit &reg)
{
    int temp = reg.value() & 1; // bit 0
    reg.set((reg.value() >> 1) | (flags.carry << 7)); // through carry
    set_cond(flags.carry, temp);
    flag_reset(reg);
}

// void Processor::RLC(Register8bit &reg)
// {
//     int temp = reg.value() >> 7; // bit 7
//     reg.set((reg.value() << 1) | temp);
//     set_cond(flags.carry, temp);
//     flag_reset(reg);
// }

void Processor::RRC(Register8bit &reg)
{
    int temp = reg.value() & 1; // bit 0
    reg.set((reg.value() >> 1) | (temp << 7));
    set_cond(flags.carry, temp);
    flag_reset(reg);
}

void Processor::SLA(Register8bit &reg)
{
    set_cond(flags.carry, reg.value() >> 7);
    reg.set(reg.value() << 1);
    flag_reset(reg);
}

void Processor::SRA(Register8bit &reg)
{
    set_cond(flags.carry, reg.value() & 1);
    u8 temp = reg.value() & (1 << 7); 
    reg.set((reg.value() >> 1) | temp);
    flag_reset(reg);
}

void Processor::SRL(Register8bit &reg)
{
    set_cond(flags.carry, reg.value() & 1);
    reg.set(reg.value() >> 1);
    flag_reset(reg);
}


// Address
void Processor::RL(Register16bit const &reg)
{
    int temp = memory[reg.value()] >> 7; // bit 7
    memory[reg.value()] = (memory[reg.value()] << 1) | (u8)flags.carry; // through carry
    set_cond(flags.carry, temp);
    flag_reset(reg);
}

void Processor::RR(Register16bit const &reg)
{
    int temp = memory[reg.value()] & 1; // bit 0
    memory[reg.value()] = (memory[reg.value()] >> 1) | (flags.carry << 7); // through carry
    set_cond(flags.carry, temp);
    flag_reset(reg);
}

void Processor::RLC(Register16bit const &reg)
{
    int temp = memory[reg.value()] >> 7; // bit 7
    memory[reg.value()] = (memory[reg.value()] << 1) | temp;
    set_cond(flags.carry, temp);
    flag_reset(reg);
}

void Processor::RRC(Register16bit const &reg)
{
    int temp = memory[reg.value()] & 1; // bit 0
    memory[reg.value()] = (memory[reg.value()] >> 1) | (temp << 7);
    set_cond(flags.carry, temp);
    flag_reset(reg);
}

void Processor::SLA(Register16bit const &reg)
{
    set_cond(flags.carry, memory[reg.value()] >> 7);
    memory[reg.value()] = memory[reg.value()] << 1;
    flag_reset(reg);
}

void Processor::SRA(Register16bit const &reg)
{
    set_cond(flags.carry, memory[reg.value()] & 1);
    u8 temp = memory[reg.value()] & (1 << 7); 
    memory[reg.value()] = (memory[reg.value()] >> 1) | temp;
    flag_reset(reg);
}

void Processor::SRL(Register16bit const &reg)
{
    set_cond(flags.carry, memory[reg.value()] & 1);
    memory[reg.value()] = memory[reg.value()] >> 1;
    flag_reset(reg);
}

#pragma endregion

void Processor::BIT(Register8bit &reg, u8 bit)
{
    set_cond(flags.zero, ((reg.value() >> bit) & 1) == 0);
    reset(flags.subtract);
    set(flags.half_carry);
}

void Processor::BIT(Register16bit const &reg, u8 bit)
{
    set_cond(flags.zero, ((memory[reg.value()] >> bit) & 1) == 0);
    reset(flags.subtract);
    set(flags.half_carry);
}

void Processor::SET(Register8bit &reg, u8 bit)
{
    reg.set(reg.value() | (1 << bit));
}

void Processor::SET(Register16bit const &reg, u8 bit)
{
    memory[reg.value()] = memory[reg.value()] | (1 << bit);
}

void Processor::RES(Register8bit &reg, u8 bit)
{
    reg.set(reg.value() & (0 << bit));
}

void Processor::RES(Register16bit const &reg, u8 bit)
{
    memory[reg.value()] = memory[reg.value()] & (0 << bit);
}


// Logic functions
#pragma region

void Processor::AND_register(Register8bit &dest, Register8bit &src)
{
    dest.set(dest.value() & src.value());
    set_and_flags(dest.value());
}

void Processor::AND_immediate(Register8bit &reg)
{
    u8 val = fetch_byte();
    reg.set(reg.value() & val);
    set_and_flags(val);
}

void Processor::AND_address(Register8bit &dest, Register16bit &src)
{
    u8 val = memory[src.value()];
    dest.set(dest.value() & val);
    set_and_flags(val);
}

void Processor::OR_register(Register8bit &dest, Register8bit &src)
{
    dest.set(dest.value() | src.value());
    set_or_flags(dest.value());
}

void Processor::OR_immediate(Register8bit &reg)
{
    u8 val = fetch_byte();
    reg.set(reg.value() | val);
    set_or_flags(val);
}

void Processor::OR_address(Register8bit &dest, Register16bit &src)
{
    u8 val = memory[src.value()];
    dest.set(dest.value() | val);
    set_or_flags(val);
}

void Processor::XOR_register(Register8bit &dest, Register8bit &src)
{
    dest.set(dest.value() ^ src.value());
    set_or_flags(dest.value());
}

void Processor::XOR_immediate(Register8bit &reg)
{
    u8 val = fetch_byte();
    reg.set(reg.value() ^ val);
    set_or_flags(val);
}

void Processor::XOR_address(Register8bit &dest, Register16bit &src)
{
    u8 val = memory[src.value()];
    dest.set(dest.value() ^ val);
    set_or_flags(val);
}

void Processor::CP_register(Register8bit &dest, Register8bit &src)
{
    set_sub_flags(dest.value(), src.value());
}

void Processor::CP_immediate(Register8bit &reg)
{
    set_sub_flags(reg.value(), fetch_byte());
}

void Processor::CP_address(Register8bit &dest, Register16bit &src)
{
    set_sub_flags(dest.value(), memory[src.value()]);
}

#pragma endregion

void Processor::SWAP(Register8bit &reg)
{
    reg.set(swap(reg.value()));
    if (reg.value() == 0) {
        set(flags.zero);
    }
    reset(flags.subtract);
    reset(flags.half_carry);
    reset(flags.carry);
}

void Processor::SWAP(Register16bit const &reg)
{
    memory[reg.value()] = swap(memory[reg.value()]);
    if (memory[reg.value()] == 0) {
        set(flags.zero);
    }
    reset(flags.subtract);
    reset(flags.half_carry);
    reset(flags.carry);
}

void Processor::RET()
{
    PC.set_low(stack_pop());
    PC.set_high(stack_pop());
}

void Processor::JP_cond(bool cond)
{
    u16 n = fetch_word();
    if (cond) {
        PC.set(n);
    }
}

void Processor::JR_cond(bool cond)
{
    i8 n = (i8)fetch_byte();
    if (cond) {
        PC.add(n);
    }
}

void Processor::CALL_cond(bool cond)
{
    u16 n = fetch_word();
    if (cond) {
        PUSH_register(PC);
        PC.set(n);
    }
}

void Processor::RST(u8 addr)
{
    PUSH_register(PC); // possibly - 1?
    PC.set(addr);
}

 8-bit load opcodes
#pragma region
void Processor::opcode0x3e() { op::LD_imm(this, A); }
void Processor::opcode0x06() { op::LD_imm(this, B); }
void Processor::opcode0x0e() { op::LD_imm(this, C); }
void Processor::opcode0x16() { op::LD_imm(this, D); }
void Processor::opcode0x1e() { op::LD_imm(this, E); }
void Processor::opcode0x26() { op::LD_imm(this, H); }
void Processor::opcode0x2e() { op::LD_imm(this, L); }

// void Processor::opcode0x40() { LD_register(B, B); }
// void Processor::opcode0x41() { LD_register(B, C); }
// void Processor::opcode0x42() { LD_register(B, D); }
// void Processor::opcode0x43() { LD_register(B, E); }
// void Processor::opcode0x44() { LD_register(B, H); }
// void Processor::opcode0x45() { LD_register(B, L); }
// void Processor::opcode0x46() { LD_address(B, HL); }
// void Processor::opcode0x47() { LD_register(B, A); }

// void Processor::opcode0x48() { LD_register(C, B); }
// void Processor::opcode0x49() { LD_register(C, C); }
// void Processor::opcode0x4a() { LD_register(C, D); }
// void Processor::opcode0x4b() { LD_register(C, E); }
// void Processor::opcode0x4c() { LD_register(C, H); }
// void Processor::opcode0x4d() { LD_register(C, L); }
// void Processor::opcode0x4e() { LD_address(C, HL); }
// void Processor::opcode0x4f() { LD_register(C, A); }

// void Processor::opcode0x50() { LD_register(D, B); }
// void Processor::opcode0x51() { LD_register(D, C); }
// void Processor::opcode0x52() { LD_register(D, D); }
// void Processor::opcode0x53() { LD_register(D, E); }
// void Processor::opcode0x54() { LD_register(D, H); }
// void Processor::opcode0x55() { LD_register(D, L); }
// void Processor::opcode0x56() { LD_address(D, HL); }
// void Processor::opcode0x57() { LD_register(D, A); }

// void Processor::opcode0x58() { LD_register(E, B); }
// void Processor::opcode0x59() { LD_register(E, C); }
// void Processor::opcode0x5a() { LD_register(E, D); }
// void Processor::opcode0x5b() { LD_register(E, E); }
// void Processor::opcode0x5c() { LD_register(E, H); }
// void Processor::opcode0x5d() { LD_register(E, L); }
// void Processor::opcode0x5e() { LD_address(E, HL); }
// void Processor::opcode0x5f() { LD_register(E, A); }

// void Processor::opcode0x60() { LD_register(H, B); }
// void Processor::opcode0x61() { LD_register(H, C); }
// void Processor::opcode0x62() { LD_register(H, D); }
// void Processor::opcode0x63() { LD_register(H, E); }
// void Processor::opcode0x64() { LD_register(H, H); }
// void Processor::opcode0x65() { LD_register(H, L); }
// void Processor::opcode0x66() { LD_address(H, HL); }
// void Processor::opcode0x67() { LD_register(H, A); }

// void Processor::opcode0x68() { LD_register(L, B); }
// void Processor::opcode0x69() { LD_register(L, C); }
// void Processor::opcode0x6a() { LD_register(L, D); }
// void Processor::opcode0x6b() { LD_register(L, E); }
// void Processor::opcode0x6c() { LD_register(L, H); }
// void Processor::opcode0x6d() { LD_register(L, L); }
// void Processor::opcode0x6e() { LD_address(L, HL); }
// void Processor::opcode0x6f() { LD_register(L, A); }

// void Processor::opcode0x70() { LD_address(HL, B); }
// void Processor::opcode0x71() { LD_address(HL, C); }
// void Processor::opcode0x72() { LD_address(HL, D); }
// void Processor::opcode0x73() { LD_address(HL, E); }
// void Processor::opcode0x74() { LD_address(HL, H); }
// void Processor::opcode0x75() { LD_address(HL, L); }
// void Processor::opcode0x36() { memory[HL.value()] = fetch_byte(); }

// void Processor::opcode0x78() { LD_register(A, B); }
// void Processor::opcode0x79() { LD_register(A, C); }
// void Processor::opcode0x7a() { LD_register(A, D); }
// void Processor::opcode0x7b() { LD_register(A, E); }
// void Processor::opcode0x7c() { LD_register(A, H); }
// void Processor::opcode0x7d() { LD_register(A, L); }
// void Processor::opcode0x7f() { LD_register(A, A); }

// void Processor::opcode0x0a() { LD_address(A, BC); }
// void Processor::opcode0x1a() { LD_address(A, DE); }
// void Processor::opcode0x7e() { LD_address(A, HL); }
// void Processor::opcode0xfa() 
// { 
//     // LD A, (nn)
//     u8 l = fetch_byte(); 
//     u8 h = fetch_byte();
//     A.set(memory[h << 8 | l]);
// }
// void Processor::opcode0x02() { LD_address(BC, A); }
// void Processor::opcode0x12() { LD_address(DE, A); }
// void Processor::opcode0x77() { LD_address(HL, A); }
// void Processor::opcode0xea() 
// { 
//     // LD (nn), A
//     u8 l = fetch_byte(); 
//     u8 h = fetch_byte();
//     memory[h << 8 | l] = A.value();
// }

// void Processor::opcode0xf2() { A.set(memory[0xff00 + C.value()]); }
// void Processor::opcode0xe2() { memory[0xff00 + C.value()] = A.value(); }

// void Processor::opcode0x3a() { LD_address(A, HL); HL.decrement(); }
// void Processor::opcode0x32() { LD_address(HL, A); HL.decrement(); }
// void Processor::opcode0x2a() { LD_address(A, HL); HL.increment(); }
// void Processor::opcode0x22() { LD_address(HL, A); HL.increment(); }

// void Processor::opcode0xe0() { memory[0xff00 + fetch_byte()] = A.value(); }
// void Processor::opcode0xf0() { A.set(memory[0xff00 + (u16)fetch_byte()]); }
// #pragma endregion


// /*	16-bit load opcodes
//  */
// #pragma region

// void Processor::opcode0x01() { LD_immediate(BC); }
// void Processor::opcode0x11() { LD_immediate(DE); }
// void Processor::opcode0x21() { LD_immediate(HL); }
// void Processor::opcode0x31() { LD_immediate(SP); }

// void Processor::opcode0xf8()
// {
//     // LD HL, SP + n
//     u8 n = fetch_byte();
//     HL.set(SP.value() + n);
//     reset(flags.zero);
//     reset(flags.subtract);
//     set_cond(flags.half_carry, half_carry_add(SP.value(), n));
//     set_cond(flags.carry, full_carry_add(SP.value(), n));
// } 

// void Processor::opcode0xf9() { LD_register(SP, HL); }

// void Processor::opcode0x08()
// {
//     // LD (nn), SP
//     u8 l = fetch_byte();
//     u8 h = fetch_byte();
//     u16 addr = h << 8 | l;
//     memory[addr] = SP.value_low();
//     memory[addr + 1] = SP.value_high();
// }

// void Processor::opcode0xf5() { PUSH_register(AF); }
// void Processor::opcode0xc5() { PUSH_register(BC); }
// void Processor::opcode0xd5() { PUSH_register(DE); }
// void Processor::opcode0xe5() { PUSH_register(HL); }

// void Processor::opcode0xf1() { POP_register(AF); }
// void Processor::opcode0xc1() { POP_register(BC); }
// void Processor::opcode0xd1() { POP_register(DE); }
// void Processor::opcode0xe1() { POP_register(HL); }

// #pragma endregion


// /*	8-bit ALU opcodes
//  */
// #pragma region
// void Processor::opcode0xc6() { ADD_immediate(A); }
// void Processor::opcode0xce() { ADD_immediate(A, true); }
// void Processor::opcode0xd6() { SUB_immediate(A); }
// void Processor::opcode0xde() { SUB_immediate(A, true); }

// void Processor::opcode0x80() { ADD_register(A, B); }
// void Processor::opcode0x81() { ADD_register(A, C); }
// void Processor::opcode0x82() { ADD_register(A, D); }
// void Processor::opcode0x83() { ADD_register(A, E); }
// void Processor::opcode0x84() { ADD_register(A, H); }
// void Processor::opcode0x85() { ADD_register(A, L); }
// void Processor::opcode0x86() { ADD_address(A, HL); }
// void Processor::opcode0x87() { ADD_register(A, A); }
// void Processor::opcode0x88() { ADD_register(A, B, true); }
// void Processor::opcode0x89() { ADD_register(A, C, true); }
// void Processor::opcode0x8a() { ADD_register(A, D, true); }
// void Processor::opcode0x8b() { ADD_register(A, E, true); }
// void Processor::opcode0x8c() { ADD_register(A, H, true); }
// void Processor::opcode0x8d() { ADD_register(A, L, true); }
// void Processor::opcode0x8e() { ADD_address(A, HL, true); }
// void Processor::opcode0x8f() { ADD_register(A, A, true); }
// void Processor::opcode0x90() { SUB_register(A, B); }
// void Processor::opcode0x91() { SUB_register(A, C); }
// void Processor::opcode0x92() { SUB_register(A, D); }
// void Processor::opcode0x93() { SUB_register(A, E); }
// void Processor::opcode0x94() { SUB_register(A, H); }
// void Processor::opcode0x95() { SUB_register(A, L); }
// void Processor::opcode0x96() { SUB_address(A, HL); }
// void Processor::opcode0x97() { SUB_register(A, A); }

// void Processor::opcode0x98() { SUB_register(A, B, true); }
// void Processor::opcode0x99() { SUB_register(A, C, true); }
// void Processor::opcode0x9a() { SUB_register(A, D, true); }
// void Processor::opcode0x9b() { SUB_register(A, E, true); }
// void Processor::opcode0x9c() { SUB_register(A, H, true); }
// void Processor::opcode0x9d() { SUB_register(A, L, true); }
// void Processor::opcode0x9e() { SUB_address(A, HL, true); }
// void Processor::opcode0x9f() { SUB_register(A, A, true); }



// void Processor::opcode0xe6() { AND_immediate(A); }

// void Processor::opcode0xa0() { AND_register(A, B); }
// void Processor::opcode0xa1() { AND_register(A, C); }
// void Processor::opcode0xa2() { AND_register(A, D); }
// void Processor::opcode0xa3() { AND_register(A, E); }
// void Processor::opcode0xa4() { AND_register(A, H); }
// void Processor::opcode0xa5() { AND_register(A, L); }
// void Processor::opcode0xa6() { AND_address(A, HL); }
// void Processor::opcode0xa7() { AND_register(A, A); }
// void Processor::opcode0xa8() { XOR_register(A, B); }
// void Processor::opcode0xa9() { XOR_register(A, C); }
// void Processor::opcode0xaa() { XOR_register(A, D); }
// void Processor::opcode0xab() { XOR_register(A, E); }
// void Processor::opcode0xac() { XOR_register(A, H); }
// void Processor::opcode0xad() { XOR_register(A, L); }
// void Processor::opcode0xae() { XOR_address(A, HL); }
// void Processor::opcode0xaf() { XOR_register(A, A); }
// void Processor::opcode0xb0() { OR_register(A, B); }
// void Processor::opcode0xb1() { OR_register(A, C); }
// void Processor::opcode0xb2() { OR_register(A, D); }
// void Processor::opcode0xb3() { OR_register(A, E); }
// void Processor::opcode0xb4() { OR_register(A, H); }
// void Processor::opcode0xb5() { OR_register(A, L); }
// void Processor::opcode0xb6() { OR_address(A, HL); }
// void Processor::opcode0xb7() { OR_register(A, A); }
// void Processor::opcode0xb8() { CP_register(A, B); }
// void Processor::opcode0xb9() { CP_register(A, C); }
// void Processor::opcode0xba() { CP_register(A, D); }
// void Processor::opcode0xbb() { CP_register(A, E); }
// void Processor::opcode0xbc() { CP_register(A, H); }
// void Processor::opcode0xbd() { CP_register(A, L); }
// void Processor::opcode0xbe() { CP_address(A, HL); }
// void Processor::opcode0xbf() { CP_register(A, A); }

// void Processor::opcode0xf6() { OR_immediate(A); }
// void Processor::opcode0xee() { XOR_immediate(A); }

// void Processor::opcode0xfe() { CP_immediate(A); }

// void Processor::opcode0x3c() { INC_register(A); }
// void Processor::opcode0x04() { INC_register(B); }
// void Processor::opcode0x0c() { INC_register(C); }
// void Processor::opcode0x14() { INC_register(D); }
// void Processor::opcode0x1c() { INC_register(E); }
// void Processor::opcode0x24() { INC_register(H); }
// void Processor::opcode0x2c() { INC_register(L); }
// void Processor::opcode0x34() { INC_address(HL); }

// void Processor::opcode0x3d() { DEC_register(A); }
// void Processor::opcode0x05() { DEC_register(B); }
// void Processor::opcode0x0d() { DEC_register(C); }
// void Processor::opcode0x15() { DEC_register(D); }
// void Processor::opcode0x1d() { DEC_register(E); }
// void Processor::opcode0x25() { DEC_register(H); }
// void Processor::opcode0x2d() { DEC_register(L); }
// void Processor::opcode0x35() { DEC_address(HL); }

// #pragma endregion


// /*	16-bit arithmetic opcodes
// */
// #pragma region

// void Processor::opcode0x09() { ADD_register(HL, BC); }
// void Processor::opcode0x19() { ADD_register(HL, DE); }
// void Processor::opcode0x29() { ADD_register(HL, HL); }
// void Processor::opcode0x39() { ADD_register(HL, SP); }
// void Processor::opcode0xe8() { ADD_immediate(SP); }

// void Processor::opcode0x03() { INC_register(BC); }
// void Processor::opcode0x13() { INC_register(DE); }
// void Processor::opcode0x23() { INC_register(HL); }
// void Processor::opcode0x33() { INC_register(SP); }

// void Processor::opcode0x0b() { DEC_register(BC); }
// void Processor::opcode0x1b() { DEC_register(DE); }
// void Processor::opcode0x2b() { DEC_register(HL); }
// void Processor::opcode0x3b() { DEC_register(SP); }

// #pragma endregion


// /*	Misc opcodes
// */
// #pragma region

// // DAA
// void Processor::opcode0x27()
// {
//     if (is_set(flags.subtract)) {
//         if (is_set(flags.carry) || A.value() > 0x99) {
//             A.add(0x60);
//             set(flags.carry);
//         }
//         if (is_set(flags.half_carry) || (A.value() & 0x0f) > 0x09) {
//             A.add(0x6);
//         }
//     } 
//     else {
//         if (is_set(flags.carry)) {
//             A.add(-0x60);
//         }
//         if (is_set(flags.half_carry)) {
//             A.add(-0x6);
//         }
//     }
//     set_cond(flags.zero, A.value() == 0);
//     reset(flags.half_carry);
// }

// // CPL
// void Processor::opcode0x2f() 
// { 
//     A.set(~A.value());
//     set(flags.subtract);
//     set(flags.half_carry);
// }

// // CCF
// void Processor::opcode0x3f()
// {
//     set_cond(flags.carry, !is_set(flags.carry));
//     reset(flags.subtract);
//     reset(flags.half_carry);
// }

// // SCF
// void Processor::opcode0x37()
// {
//     reset(flags.subtract);
//     reset(flags.half_carry);
//     set(flags.carry);
// }

// // HALT
// void Processor::opcode0x76()
// {

// }

// // STOP
// void Processor::opcode0x10()
// {

// }

// // DI
// void Processor::opcode0xf3()
// {
//     IME_flag = false;
// }

// // EI
// void Processor::opcode0xfb()
// {
//     IME_flag = true;
// }

// #pragma endregion


// /*	Rotate/Shift opcodes
// */
// #pragma region

// void Processor::opcode0x07() { RLC(A); }
// void Processor::opcode0x0f() { RRC(A); }
// void Processor::opcode0x17() { RL(A); }
// void Processor::opcode0x1f() { RR(A); }

// void Processor::cb_opcode0x00() { RLC(B); }
// void Processor::cb_opcode0x01() { RLC(C); }
// void Processor::cb_opcode0x02() { RLC(D); }
// void Processor::cb_opcode0x03() { RLC(E); }
// void Processor::cb_opcode0x04() { RLC(H); }
// void Processor::cb_opcode0x05() { RLC(L); }
// void Processor::cb_opcode0x06() { RLC(HL); }
// void Processor::cb_opcode0x07() { RLC(A); }
// void Processor::cb_opcode0x08() { RRC(B); }
// void Processor::cb_opcode0x09() { RRC(C); }
// void Processor::cb_opcode0x0a() { RRC(D); }
// void Processor::cb_opcode0x0b() { RRC(E); }
// void Processor::cb_opcode0x0c() { RRC(H); }
// void Processor::cb_opcode0x0d() { RRC(L); }
// void Processor::cb_opcode0x0e() { RRC(HL); }
// void Processor::cb_opcode0x0f() { RRC(A); }
// void Processor::cb_opcode0x10() { RL(B); }
// void Processor::cb_opcode0x11() { RL(C); }
// void Processor::cb_opcode0x12() { RL(D); }
// void Processor::cb_opcode0x13() { RL(E); }
// void Processor::cb_opcode0x14() { RL(H); }
// void Processor::cb_opcode0x15() { RL(L); }
// void Processor::cb_opcode0x16() { RL(HL); }
// void Processor::cb_opcode0x17() { RL(A); }
// void Processor::cb_opcode0x18() { RR(B); }
// void Processor::cb_opcode0x19() { RR(C); }
// void Processor::cb_opcode0x1a() { RR(D); }
// void Processor::cb_opcode0x1b() { RR(E); }
// void Processor::cb_opcode0x1c() { RR(H); }
// void Processor::cb_opcode0x1d() { RR(L); }
// void Processor::cb_opcode0x1e() { RR(HL); }
// void Processor::cb_opcode0x1f() { RR(A); }
// void Processor::cb_opcode0x20() { SLA(B); }
// void Processor::cb_opcode0x21() { SLA(C); }
// void Processor::cb_opcode0x22() { SLA(D); }
// void Processor::cb_opcode0x23() { SLA(E); }
// void Processor::cb_opcode0x24() { SLA(H); }
// void Processor::cb_opcode0x25() { SLA(L); }
// void Processor::cb_opcode0x26() { SLA(HL); }
// void Processor::cb_opcode0x27() { SLA(A); }
// void Processor::cb_opcode0x28() { SRA(B); }
// void Processor::cb_opcode0x29() { SRA(C); }
// void Processor::cb_opcode0x2a() { SRA(D); }
// void Processor::cb_opcode0x2b() { SRA(E); }
// void Processor::cb_opcode0x2c() { SRA(H); }
// void Processor::cb_opcode0x2d() { SRA(L); }
// void Processor::cb_opcode0x2e() { SRA(HL); }
// void Processor::cb_opcode0x2f() { SRA(A); }
// void Processor::cb_opcode0x30() { SWAP(B); } 
// void Processor::cb_opcode0x31() { SWAP(C); } 
// void Processor::cb_opcode0x32() { SWAP(D); } 
// void Processor::cb_opcode0x33() { SWAP(E); } 
// void Processor::cb_opcode0x34() { SWAP(H); } 
// void Processor::cb_opcode0x35() { SWAP(L); } 
// void Processor::cb_opcode0x36() { SWAP(HL); }
// void Processor::cb_opcode0x37() { SWAP(A); } 
// void Processor::cb_opcode0x38() { SRL(B); }
// void Processor::cb_opcode0x39() { SRL(C); }
// void Processor::cb_opcode0x3a() { SRL(D); }
// void Processor::cb_opcode0x3b() { SRL(E); }
// void Processor::cb_opcode0x3c() { SRL(H); }
// void Processor::cb_opcode0x3d() { SRL(L); }
// void Processor::cb_opcode0x3e() { SRL(HL); }
// void Processor::cb_opcode0x3f() { SRL(A); }
// void Processor::cb_opcode0x40() { BIT(B, 0); } 
// void Processor::cb_opcode0x41() { BIT(C, 0); } 
// void Processor::cb_opcode0x42() { BIT(D, 0); } 
// void Processor::cb_opcode0x43() { BIT(E, 0); } 
// void Processor::cb_opcode0x44() { BIT(H, 0); } 
// void Processor::cb_opcode0x45() { BIT(L, 0); } 
// void Processor::cb_opcode0x46() { BIT(HL, 0); }
// void Processor::cb_opcode0x47() { BIT(A, 0); } 
// void Processor::cb_opcode0x48() { BIT(B, 1); } 
// void Processor::cb_opcode0x49() { BIT(C, 1); } 
// void Processor::cb_opcode0x4a() { BIT(D, 1); } 
// void Processor::cb_opcode0x4b() { BIT(E, 1); } 
// void Processor::cb_opcode0x4c() { BIT(H, 1); } 
// void Processor::cb_opcode0x4d() { BIT(L, 1); } 
// void Processor::cb_opcode0x4e() { BIT(HL, 1); }
// void Processor::cb_opcode0x4f() { BIT(A, 1); } 
// void Processor::cb_opcode0x50() { BIT(B, 2); } 
// void Processor::cb_opcode0x51() { BIT(C, 2); } 
// void Processor::cb_opcode0x52() { BIT(D, 2); } 
// void Processor::cb_opcode0x53() { BIT(E, 2); } 
// void Processor::cb_opcode0x54() { BIT(H, 2); } 
// void Processor::cb_opcode0x55() { BIT(L, 2); } 
// void Processor::cb_opcode0x56() { BIT(HL, 2); }
// void Processor::cb_opcode0x57() { BIT(A, 2); } 
// void Processor::cb_opcode0x58() { BIT(B, 3); } 
// void Processor::cb_opcode0x59() { BIT(C, 3); } 
// void Processor::cb_opcode0x5a() { BIT(D, 3); } 
// void Processor::cb_opcode0x5b() { BIT(E, 3); } 
// void Processor::cb_opcode0x5c() { BIT(H, 3); } 
// void Processor::cb_opcode0x5d() { BIT(L, 3); } 
// void Processor::cb_opcode0x5e() { BIT(HL, 3); }
// void Processor::cb_opcode0x5f() { BIT(A, 3); } 
// void Processor::cb_opcode0x60() { BIT(B, 4); } 
// void Processor::cb_opcode0x61() { BIT(C, 4); } 
// void Processor::cb_opcode0x62() { BIT(D, 4); } 
// void Processor::cb_opcode0x63() { BIT(E, 4); } 
// void Processor::cb_opcode0x64() { BIT(H, 4); } 
// void Processor::cb_opcode0x65() { BIT(L, 4); } 
// void Processor::cb_opcode0x66() { BIT(HL, 4); }
// void Processor::cb_opcode0x67() { BIT(A, 4); } 
// void Processor::cb_opcode0x68() { BIT(B, 5); } 
// void Processor::cb_opcode0x69() { BIT(C, 5); } 
// void Processor::cb_opcode0x6a() { BIT(D, 5); } 
// void Processor::cb_opcode0x6b() { BIT(E, 5); } 
// void Processor::cb_opcode0x6c() { BIT(H, 5); } 
// void Processor::cb_opcode0x6d() { BIT(L, 5); } 
// void Processor::cb_opcode0x6e() { BIT(HL, 5); }
// void Processor::cb_opcode0x6f() { BIT(A, 5); } 
// void Processor::cb_opcode0x70() { BIT(B, 6); } 
// void Processor::cb_opcode0x71() { BIT(C, 6); } 
// void Processor::cb_opcode0x72() { BIT(D, 6); } 
// void Processor::cb_opcode0x73() { BIT(E, 6); } 
// void Processor::cb_opcode0x74() { BIT(H, 6); } 
// void Processor::cb_opcode0x75() { BIT(L, 6); } 
// void Processor::cb_opcode0x76() { BIT(HL, 6); }
// void Processor::cb_opcode0x77() { BIT(A, 6); } 
// void Processor::cb_opcode0x78() { BIT(B, 7); } 
// void Processor::cb_opcode0x79() { BIT(C, 7); } 
// void Processor::cb_opcode0x7a() { BIT(D, 7); } 
// void Processor::cb_opcode0x7b() { BIT(E, 7); } 
// void Processor::cb_opcode0x7c() { BIT(H, 7); } 
// void Processor::cb_opcode0x7d() { BIT(L, 7); } 
// void Processor::cb_opcode0x7e() { BIT(HL, 7); }
// void Processor::cb_opcode0x7f() { BIT(A, 7); } 
// void Processor::cb_opcode0x80() { RES(B, 0); }
// void Processor::cb_opcode0x81() { RES(C, 0); }
// void Processor::cb_opcode0x82() { RES(D, 0); }
// void Processor::cb_opcode0x83() { RES(E, 0); }
// void Processor::cb_opcode0x84() { RES(H, 0); }
// void Processor::cb_opcode0x85() { RES(L, 0); }
// void Processor::cb_opcode0x86() { RES(HL, 0); }
// void Processor::cb_opcode0x87() { RES(A, 0); }
// void Processor::cb_opcode0x88() { RES(B, 1); }
// void Processor::cb_opcode0x89() { RES(C, 1); }
// void Processor::cb_opcode0x8a() { RES(D, 1); }
// void Processor::cb_opcode0x8b() { RES(E, 1); }
// void Processor::cb_opcode0x8c() { RES(H, 1); }
// void Processor::cb_opcode0x8d() { RES(L, 1); }
// void Processor::cb_opcode0x8e() { RES(HL, 1); }
// void Processor::cb_opcode0x8f() { RES(A, 1); }
// void Processor::cb_opcode0x90() { RES(B, 2); }
// void Processor::cb_opcode0x91() { RES(C, 2); }
// void Processor::cb_opcode0x92() { RES(D, 2); }
// void Processor::cb_opcode0x93() { RES(E, 2); }
// void Processor::cb_opcode0x94() { RES(H, 2); }
// void Processor::cb_opcode0x95() { RES(L, 2); }
// void Processor::cb_opcode0x96() { RES(HL, 2); }
// void Processor::cb_opcode0x97() { RES(A, 2); }
// void Processor::cb_opcode0x98() { RES(B, 3); }
// void Processor::cb_opcode0x99() { RES(C, 3); }
// void Processor::cb_opcode0x9a() { RES(D, 3); }
// void Processor::cb_opcode0x9b() { RES(E, 3); }
// void Processor::cb_opcode0x9c() { RES(H, 3); }
// void Processor::cb_opcode0x9d() { RES(L, 3); }
// void Processor::cb_opcode0x9e() { RES(HL, 3); }
// void Processor::cb_opcode0x9f() { RES(A, 3); }
// void Processor::cb_opcode0xa0() { RES(B, 4); }
// void Processor::cb_opcode0xa1() { RES(C, 4); }
// void Processor::cb_opcode0xa2() { RES(D, 4); }
// void Processor::cb_opcode0xa3() { RES(E, 4); }
// void Processor::cb_opcode0xa4() { RES(H, 4); }
// void Processor::cb_opcode0xa5() { RES(L, 4); }
// void Processor::cb_opcode0xa6() { RES(HL, 4); }
// void Processor::cb_opcode0xa7() { RES(A, 4); }
// void Processor::cb_opcode0xa8() { RES(B, 5); }
// void Processor::cb_opcode0xa9() { RES(C, 5); }
// void Processor::cb_opcode0xaa() { RES(D, 5); }
// void Processor::cb_opcode0xab() { RES(E, 5); }
// void Processor::cb_opcode0xac() { RES(H, 5); }
// void Processor::cb_opcode0xad() { RES(L, 5); }
// void Processor::cb_opcode0xae() { RES(HL, 5); }
// void Processor::cb_opcode0xaf() { RES(A, 5); }
// void Processor::cb_opcode0xb0() { RES(B, 6); }
// void Processor::cb_opcode0xb1() { RES(C, 6); }
// void Processor::cb_opcode0xb2() { RES(D, 6); }
// void Processor::cb_opcode0xb3() { RES(E, 6); }
// void Processor::cb_opcode0xb4() { RES(H, 6); }
// void Processor::cb_opcode0xb5() { RES(L, 6); }
// void Processor::cb_opcode0xb6() { RES(HL, 6); }
// void Processor::cb_opcode0xb7() { RES(A, 6); }
// void Processor::cb_opcode0xb8() { RES(B, 7); }
// void Processor::cb_opcode0xb9() { RES(C, 7); }
// void Processor::cb_opcode0xba() { RES(D, 7); }
// void Processor::cb_opcode0xbb() { RES(E, 7); }
// void Processor::cb_opcode0xbc() { RES(H, 7); }
// void Processor::cb_opcode0xbd() { RES(L, 7); }
// void Processor::cb_opcode0xbe() { RES(HL, 7); }
// void Processor::cb_opcode0xbf() { RES(A, 7); }
// void Processor::cb_opcode0xc0() { SET(B, 0); }
// void Processor::cb_opcode0xc1() { SET(C, 0); }
// void Processor::cb_opcode0xc2() { SET(D, 0); }
// void Processor::cb_opcode0xc3() { SET(E, 0); }
// void Processor::cb_opcode0xc4() { SET(H, 0); }
// void Processor::cb_opcode0xc5() { SET(L, 0); }
// void Processor::cb_opcode0xc6() { SET(HL, 0); }
// void Processor::cb_opcode0xc7() { SET(A, 0); }
// void Processor::cb_opcode0xc8() { SET(B, 1); }
// void Processor::cb_opcode0xc9() { SET(C, 1); }
// void Processor::cb_opcode0xca() { SET(D, 1); }
// void Processor::cb_opcode0xcb() { SET(E, 1); }
// void Processor::cb_opcode0xcc() { SET(H, 1); }
// void Processor::cb_opcode0xcd() { SET(L, 1); }
// void Processor::cb_opcode0xce() { SET(HL, 1); }
// void Processor::cb_opcode0xcf() { SET(A, 1); }
// void Processor::cb_opcode0xd0() { SET(B, 2); }
// void Processor::cb_opcode0xd1() { SET(C, 2); }
// void Processor::cb_opcode0xd2() { SET(D, 2); }
// void Processor::cb_opcode0xd3() { SET(E, 2); }
// void Processor::cb_opcode0xd4() { SET(H, 2); }
// void Processor::cb_opcode0xd5() { SET(L, 2); }
// void Processor::cb_opcode0xd6() { SET(HL, 2); }
// void Processor::cb_opcode0xd7() { SET(A, 2); }
// void Processor::cb_opcode0xd8() { SET(B, 3); }
// void Processor::cb_opcode0xd9() { SET(C, 3); }
// void Processor::cb_opcode0xda() { SET(D, 3); }
// void Processor::cb_opcode0xdb() { SET(E, 3); }
// void Processor::cb_opcode0xdc() { SET(H, 3); }
// void Processor::cb_opcode0xdd() { SET(L, 3); }
// void Processor::cb_opcode0xde() { SET(HL, 3); }
// void Processor::cb_opcode0xdf() { SET(A, 3); }
// void Processor::cb_opcode0xe0() { SET(B, 4); }
// void Processor::cb_opcode0xe1() { SET(C, 4); }
// void Processor::cb_opcode0xe2() { SET(D, 4); }
// void Processor::cb_opcode0xe3() { SET(E, 4); }
// void Processor::cb_opcode0xe4() { SET(H, 4); }
// void Processor::cb_opcode0xe5() { SET(L, 4); }
// void Processor::cb_opcode0xe6() { SET(HL, 4); }
// void Processor::cb_opcode0xe7() { SET(A, 4); }
// void Processor::cb_opcode0xe8() { SET(B, 5); }
// void Processor::cb_opcode0xe9() { SET(C, 5); }
// void Processor::cb_opcode0xea() { SET(D, 5); }
// void Processor::cb_opcode0xeb() { SET(E, 5); }
// void Processor::cb_opcode0xec() { SET(H, 5); }
// void Processor::cb_opcode0xed() { SET(L, 5); }
// void Processor::cb_opcode0xee() { SET(HL, 5); }
// void Processor::cb_opcode0xef() { SET(A, 5); }
// void Processor::cb_opcode0xf0() { SET(B, 6); }
// void Processor::cb_opcode0xf1() { SET(C, 6); }
// void Processor::cb_opcode0xf2() { SET(D, 6); }
// void Processor::cb_opcode0xf3() { SET(E, 6); }
// void Processor::cb_opcode0xf4() { SET(H, 6); }
// void Processor::cb_opcode0xf5() { SET(L, 6); }
// void Processor::cb_opcode0xf6() { SET(HL, 6); }
// void Processor::cb_opcode0xf7() { SET(A, 6); }
// void Processor::cb_opcode0xf8() { SET(B, 7); }
// void Processor::cb_opcode0xf9() { SET(C, 7); }
// void Processor::cb_opcode0xfa() { SET(D, 7); }
// void Processor::cb_opcode0xfb() { SET(E, 7); }
// void Processor::cb_opcode0xfc() { SET(H, 7); }
// void Processor::cb_opcode0xfd() { SET(L, 7); }
// void Processor::cb_opcode0xfe() { SET(HL, 7); }
// void Processor::cb_opcode0xff() { SET(A, 7); }

// #pragma endregion


// /*	Bit opcodes
// */
// #pragma region


                                               




// #pragma endregion


// /* Jump opcodes
// */
// #pragma region

// // JP nn
// void Processor::opcode0xc3() { PC.set(fetch_word()); }

// void Processor::opcode0xc2() { JP_cond(!is_set(flags.zero)); }
// void Processor::opcode0xca() { JP_cond(is_set(flags.zero)); }
// void Processor::opcode0xd2() { JP_cond(!is_set(flags.carry)); }
// void Processor::opcode0xda() { JP_cond(is_set(flags.carry)); }

// void Processor::opcode0xe9() { PC.set(memory[HL.value()]); }

// void Processor::opcode0x18() { JR_cond(true); }
// void Processor::opcode0x20() { JR_cond(!is_set(flags.zero)); }
// void Processor::opcode0x28() { JR_cond(is_set(flags.zero)); }
// void Processor::opcode0x30() { JR_cond(!is_set(flags.carry)); }
// void Processor::opcode0x38() { JR_cond(is_set(flags.carry)); }

// #pragma endregion

// /* Call/Return opcodes
// */
// #pragma region

// // CALL
// void Processor::opcode0xcd()
// {
//     u16 n = fetch_word();
//     PUSH_register(PC);
//     PC.set(n);
// }

// void Processor::opcode0xc4() { CALL_cond(!is_set(flags.zero)); }
// void Processor::opcode0xcc() { CALL_cond(is_set(flags.zero)); }
// void Processor::opcode0xd4() { CALL_cond(!is_set(flags.carry)); }
// void Processor::opcode0xdc() { CALL_cond(is_set(flags.carry)); }

// void Processor::opcode0xc9() { RET(); }
// void Processor::opcode0xc0() { if (!is_set(flags.zero)) RET(); }
// void Processor::opcode0xc8() { if (is_set(flags.zero)) RET(); }
// void Processor::opcode0xd0() { if (!is_set(flags.carry)) RET(); }
// void Processor::opcode0xd8() { if (is_set(flags.carry)) RET(); }

// void Processor::opcode0xd9()
// {
//     RET();
//     //EI
//     opcode0xfb();
// }

// void Processor::opcode0xc7() { RST(0x00); }
// void Processor::opcode0xcf() { RST(0x08); }
// void Processor::opcode0xd7() { RST(0x10); }
// void Processor::opcode0xdf() { RST(0x18); }
// void Processor::opcode0xe7() { RST(0x20); }
// void Processor::opcode0xef() { RST(0x28); }
// void Processor::opcode0xf7() { RST(0x30); }
// void Processor::opcode0xff() { RST(0x38); }

// #pragma endregion




