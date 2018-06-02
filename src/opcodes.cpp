#include "processor.h"
#include <cassert>
#include <iostream>

//Private functions
#pragma region

u8 Processor::fetch_byte()
{
    u8 data = memory[PC.value()];
    PC.increment();
    return data;
}
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
    if (reg.value() == 0)
        set(flags.zero);
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

void Processor::LD_immediate(Register8bit &reg)
{
    reg.set(fetch_byte());
}

void Processor::LD_immediate(Register16bit &reg)
{
    u8 l = fetch_byte(); 
    u8 h = fetch_byte();
    reg.set_low(l);
    reg.set_high(h);
}

void Processor::LD_address(Register8bit &dest, Register16bit const &src)
{
    dest.set(memory[src.value()]);
}

void Processor::LD_address(Register16bit const &dest, Register8bit const &src)
{
    memory[dest.value()] = src.value();
}

void Processor::LD_register(Register8bit &dest, Register8bit const &src)
{
    dest.set(src.value());
}

void Processor::LD_register(Register16bit &dest, Register16bit const &src)
{
    dest.set(src.value());
}

void Processor::PUSH_register(Register8bit const &reg)
{
    stack_push(reg.value());
}

void Processor::PUSH_register(Register16bit const &reg)
{
    stack_push(reg.value_high());
    stack_push(reg.value_low());
}

void Processor::POP_register(Register8bit &reg)
{
    reg.set(stack_pop());
}

void Processor::POP_register(Register16bit &reg)
{
    reg.set_low(stack_pop());
    reg.set_high(stack_pop());
}

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

void Processor::RL_carry(Register8bit &reg)
{
    int temp = reg.value() >> 7; // bit 7
    reg.set((reg.value() << 1) | (u8)flags.carry);
    set_cond(flags.carry, temp);
    flag_reset(reg);
}

void Processor::RR_carry(Register8bit &reg)
{
    int temp = reg.value() & 1; // bit 0
    reg.set((reg.value() >> 1) | (flags.carry << 7));
    set_cond(flags.carry, temp);
    flag_reset(reg);
}

void Processor::RL_no_carry(Register8bit &reg)
{
    int temp = reg.value() >> 7; // bit 7
    reg.set((reg.value() << 1) | temp);
    set_cond(flags.carry, temp);
    flag_reset(reg);
}

void Processor::RR_no_carry(Register8bit &reg)
{
    int temp = reg.value() & 1; // bit 0
    reg.set((reg.value() >> 1) | (temp << 7));
    set_cond(flags.carry, temp);
    flag_reset(reg);
}

#pragma endregion


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

void Processor::SWAP_register(Register8bit &reg)
{
    reg.set(swap(reg.value()));
    if (reg.value() == 0) {
        set(flags.zero);
    }
    reset(flags.subtract);
    reset(flags.half_carry);
    reset(flags.carry);
}

void Processor::SWAP_address(Register16bit const &reg)
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

//  8-bit load opcodes
#pragma region
void Processor::opcode0x3e() { LD_immediate(A); }
void Processor::opcode0x06() { LD_immediate(B); }
void Processor::opcode0x0e() { LD_immediate(C); }
void Processor::opcode0x16() { LD_immediate(D); }
void Processor::opcode0x1e() { LD_immediate(E); }
void Processor::opcode0x26() { LD_immediate(H); }
void Processor::opcode0x2e() { LD_immediate(L); }

void Processor::opcode0x40() { LD_register(B, B); }
void Processor::opcode0x41() { LD_register(B, C); }
void Processor::opcode0x42() { LD_register(B, D); }
void Processor::opcode0x43() { LD_register(B, E); }
void Processor::opcode0x44() { LD_register(B, H); }
void Processor::opcode0x45() { LD_register(B, L); }
void Processor::opcode0x46() { LD_address(B, HL); }
void Processor::opcode0x47() { LD_register(B, A); }

void Processor::opcode0x48() { LD_register(C, B); }
void Processor::opcode0x49() { LD_register(C, C); }
void Processor::opcode0x4a() { LD_register(C, D); }
void Processor::opcode0x4b() { LD_register(C, E); }
void Processor::opcode0x4c() { LD_register(C, H); }
void Processor::opcode0x4d() { LD_register(C, L); }
void Processor::opcode0x4e() { LD_address(C, HL); }
void Processor::opcode0x4f() { LD_register(C, A); }

void Processor::opcode0x50() { LD_register(D, B); }
void Processor::opcode0x51() { LD_register(D, C); }
void Processor::opcode0x52() { LD_register(D, D); }
void Processor::opcode0x53() { LD_register(D, E); }
void Processor::opcode0x54() { LD_register(D, H); }
void Processor::opcode0x55() { LD_register(D, L); }
void Processor::opcode0x56() { LD_address(D, HL); }
void Processor::opcode0x57() { LD_register(D, A); }

void Processor::opcode0x58() { LD_register(E, B); }
void Processor::opcode0x59() { LD_register(E, C); }
void Processor::opcode0x5a() { LD_register(E, D); }
void Processor::opcode0x5b() { LD_register(E, E); }
void Processor::opcode0x5c() { LD_register(E, H); }
void Processor::opcode0x5d() { LD_register(E, L); }
void Processor::opcode0x5e() { LD_address(E, HL); }
void Processor::opcode0x5f() { LD_register(E, A); }

void Processor::opcode0x60() { LD_register(H, B); }
void Processor::opcode0x61() { LD_register(H, C); }
void Processor::opcode0x62() { LD_register(H, D); }
void Processor::opcode0x63() { LD_register(H, E); }
void Processor::opcode0x64() { LD_register(H, H); }
void Processor::opcode0x65() { LD_register(H, L); }
void Processor::opcode0x66() { LD_address(H, HL); }
void Processor::opcode0x67() { LD_register(H, A); }

void Processor::opcode0x68() { LD_register(L, B); }
void Processor::opcode0x69() { LD_register(L, C); }
void Processor::opcode0x6a() { LD_register(L, D); }
void Processor::opcode0x6b() { LD_register(L, E); }
void Processor::opcode0x6c() { LD_register(L, H); }
void Processor::opcode0x6d() { LD_register(L, L); }
void Processor::opcode0x6e() { LD_address(L, HL); }
void Processor::opcode0x6f() { LD_register(L, A); }

void Processor::opcode0x70() { LD_address(HL, B); }
void Processor::opcode0x71() { LD_address(HL, C); }
void Processor::opcode0x72() { LD_address(HL, D); }
void Processor::opcode0x73() { LD_address(HL, E); }
void Processor::opcode0x74() { LD_address(HL, H); }
void Processor::opcode0x75() { LD_address(HL, L); }
void Processor::opcode0x36() { memory[HL.value()] = fetch_byte(); }

void Processor::opcode0x78() { LD_register(A, B); }
void Processor::opcode0x79() { LD_register(A, C); }
void Processor::opcode0x7a() { LD_register(A, D); }
void Processor::opcode0x7b() { LD_register(A, E); }
void Processor::opcode0x7c() { LD_register(A, H); }
void Processor::opcode0x7d() { LD_register(A, L); }
void Processor::opcode0x7f() { LD_register(A, A); }

void Processor::opcode0x0a() { LD_address(A, BC); }
void Processor::opcode0x1a() { LD_address(A, DE); }
void Processor::opcode0x7e() { LD_address(A, HL); }
void Processor::opcode0xfa() 
{ 
    // LD A, (nn)
    u8 l = fetch_byte(); 
    u8 h = fetch_byte();
    A.set(memory[h << 8 | l]);
}
void Processor::opcode0x02() { LD_address(BC, A); }
void Processor::opcode0x12() { LD_address(DE, A); }
void Processor::opcode0x77() { LD_address(HL, A); }
void Processor::opcode0xea() 
{ 
    // LD (nn), A
    u8 l = fetch_byte(); 
    u8 h = fetch_byte();
    memory[h << 8 | l] = A.value();
}

void Processor::opcode0xf2() { A.set(memory[0xff00 + C.value()]); }
void Processor::opcode0xe2() { memory[0xff00 + C.value()] = A.value(); }

void Processor::opcode0x3a() { LD_address(A, HL); HL.decrement(); }
void Processor::opcode0x32() { LD_address(HL, A); HL.decrement(); }
void Processor::opcode0x2a() { LD_address(A, HL); HL.increment(); }
void Processor::opcode0x22() { LD_address(HL, A); HL.increment(); }

void Processor::opcode0xe0() { memory[0xff00 + fetch_byte()] = A.value(); }
void Processor::opcode0xf0() { A.set(memory[0xff00 + fetch_byte()]); }
#pragma endregion


/*	16-bit load opcodes
 */
#pragma region

void Processor::opcode0x01() { LD_immediate(BC); }
void Processor::opcode0x11() { LD_immediate(DE); }
void Processor::opcode0x21() { LD_immediate(HL); }
void Processor::opcode0x31() { LD_immediate(SP); }

void Processor::opcode0xf8()
{
    // LD HL, SP + n
    u8 n = fetch_byte();
    HL.set(SP.value() + n);
    reset(flags.zero);
    reset(flags.subtract);
    set_cond(flags.half_carry, half_carry_add(SP.value(), n));
    set_cond(flags.carry, full_carry_add(SP.value(), n));
} 

void Processor::opcode0xf9() { LD_register(SP, HL); }

void Processor::opcode0x08()
{
    // LD (nn), SP
    u8 l = fetch_byte();
    u8 h = fetch_byte();
    u16 addr = h << 8 | l;
    memory[addr] = SP.value_low();
    memory[addr + 1] = SP.value_high();
}

void Processor::opcode0xf5() { PUSH_register(AF); }
void Processor::opcode0xc5() { PUSH_register(BC); }
void Processor::opcode0xd5() { PUSH_register(DE); }
void Processor::opcode0xe5() { PUSH_register(HL); }

void Processor::opcode0xf1() { POP_register(AF); }
void Processor::opcode0xc1() { POP_register(BC); }
void Processor::opcode0xd1() { POP_register(DE); }
void Processor::opcode0xe1() { POP_register(HL); }

#pragma endregion


/*	8-bit ALU opcodes
 */
#pragma region

void Processor::opcode0x87() { ADD_register(A, A); }
void Processor::opcode0x80() { ADD_register(A, B); }
void Processor::opcode0x81() { ADD_register(A, C); }
void Processor::opcode0x82() { ADD_register(A, D); }
void Processor::opcode0x83() { ADD_register(A, E); }
void Processor::opcode0x84() { ADD_register(A, H); }
void Processor::opcode0x85() { ADD_register(A, L); }
void Processor::opcode0x86() { ADD_address(A, HL); }
void Processor::opcode0xc6() { ADD_immediate(A); }

void Processor::opcode0x8f() { ADD_register(A, A, true); }
void Processor::opcode0x88() { ADD_register(A, B, true); }
void Processor::opcode0x89() { ADD_register(A, C, true); }
void Processor::opcode0x8a() { ADD_register(A, D, true); }
void Processor::opcode0x8b() { ADD_register(A, E, true); }
void Processor::opcode0x8c() { ADD_register(A, H, true); }
void Processor::opcode0x8d() { ADD_register(A, L, true); }
void Processor::opcode0x8e() { ADD_address(A, HL, true); }
void Processor::opcode0xce() { ADD_immediate(A, true); }

void Processor::opcode0x97() { SUB_register(A, A); }
void Processor::opcode0x90() { SUB_register(A, B); }
void Processor::opcode0x91() { SUB_register(A, C); }
void Processor::opcode0x92() { SUB_register(A, D); }
void Processor::opcode0x93() { SUB_register(A, E); }
void Processor::opcode0x94() { SUB_register(A, H); }
void Processor::opcode0x95() { SUB_register(A, L); }
void Processor::opcode0x96() { SUB_address(A, HL); }

void Processor::opcode0x9f() { SUB_register(A, A, true); }
void Processor::opcode0x98() { SUB_register(A, B, true); }
void Processor::opcode0x99() { SUB_register(A, C, true); }
void Processor::opcode0x9a() { SUB_register(A, D, true); }
void Processor::opcode0x9b() { SUB_register(A, E, true); }
void Processor::opcode0x9c() { SUB_register(A, H, true); }
void Processor::opcode0x9d() { SUB_register(A, L, true); }
void Processor::opcode0x9e() { SUB_address(A, HL, true); }

void Processor::opcode0xa7() { AND_register(A, A); }
void Processor::opcode0xa0() { AND_register(A, B); }
void Processor::opcode0xa1() { AND_register(A, C); }
void Processor::opcode0xa2() { AND_register(A, D); }
void Processor::opcode0xa3() { AND_register(A, E); }
void Processor::opcode0xa4() { AND_register(A, H); }
void Processor::opcode0xa5() { AND_register(A, L); }
void Processor::opcode0xa6() { AND_address(A, HL); }
void Processor::opcode0xe6() { AND_immediate(A); }

void Processor::opcode0xb7() { OR_register(A, A); }
void Processor::opcode0xb0() { OR_register(A, B); }
void Processor::opcode0xb1() { OR_register(A, C); }
void Processor::opcode0xb2() { OR_register(A, D); }
void Processor::opcode0xb3() { OR_register(A, E); }
void Processor::opcode0xb4() { OR_register(A, H); }
void Processor::opcode0xb5() { OR_register(A, L); }
void Processor::opcode0xb6() { OR_address(A, HL); }
void Processor::opcode0xf6() { OR_immediate(A); }

void Processor::opcode0xaf() { XOR_register(A, A); }
void Processor::opcode0xa8() { XOR_register(A, B); }
void Processor::opcode0xa9() { XOR_register(A, C); }
void Processor::opcode0xaa() { XOR_register(A, D); }
void Processor::opcode0xab() { XOR_register(A, E); }
void Processor::opcode0xac() { XOR_register(A, H); }
void Processor::opcode0xad() { XOR_register(A, L); }
void Processor::opcode0xae() { XOR_address(A, HL); }
void Processor::opcode0xee() { XOR_immediate(A); }

void Processor::opcode0xbf() { CP_register(A, A); }
void Processor::opcode0xb8() { CP_register(A, B); }
void Processor::opcode0xb9() { CP_register(A, C); }
void Processor::opcode0xba() { CP_register(A, D); }
void Processor::opcode0xbb() { CP_register(A, E); }
void Processor::opcode0xbc() { CP_register(A, H); }
void Processor::opcode0xbd() { CP_register(A, L); }
void Processor::opcode0xbe() { CP_address(A, HL); }
void Processor::opcode0xfe() { CP_immediate(A); }

void Processor::opcode0x3c() { INC_register(A); }
void Processor::opcode0x04() { INC_register(B); }
void Processor::opcode0x0c() { INC_register(C); }
void Processor::opcode0x14() { INC_register(D); }
void Processor::opcode0x1c() { INC_register(E); }
void Processor::opcode0x24() { INC_register(H); }
void Processor::opcode0x2c() { INC_register(L); }
void Processor::opcode0x34() { INC_address(HL); }

void Processor::opcode0x3d() { DEC_register(A); }
void Processor::opcode0x05() { DEC_register(B); }
void Processor::opcode0x0d() { DEC_register(C); }
void Processor::opcode0x15() { DEC_register(D); }
void Processor::opcode0x1d() { DEC_register(E); }
void Processor::opcode0x25() { DEC_register(H); }
void Processor::opcode0x2d() { DEC_register(L); }
void Processor::opcode0x35() { DEC_address(HL); }

#pragma endregion


/*	16-bit arithmetic opcodes
*/
#pragma region

void Processor::opcode0x09() { ADD_register(HL, BC); }
void Processor::opcode0x19() { ADD_register(HL, DE); }
void Processor::opcode0x29() { ADD_register(HL, HL); }
void Processor::opcode0x39() { ADD_register(HL, SP); }
void Processor::opcode0xe8() { ADD_immediate(SP); }

void Processor::opcode0x03() { INC_register(BC); }
void Processor::opcode0x13() { INC_register(DE); }
void Processor::opcode0x23() { INC_register(HL); }
void Processor::opcode0x33() { INC_register(SP); }

void Processor::opcode0x0b() { DEC_register(BC); }
void Processor::opcode0x1b() { DEC_register(DE); }
void Processor::opcode0x2b() { DEC_register(HL); }
void Processor::opcode0x3b() { DEC_register(SP); }

#pragma endregion


/*	Misc opcodes
*/
#pragma region

void Processor::cb_opcode0x37() { SWAP_register(A); }
void Processor::cb_opcode0x30() { SWAP_register(B); }
void Processor::cb_opcode0x31() { SWAP_register(C); }
void Processor::cb_opcode0x32() { SWAP_register(D); }
void Processor::cb_opcode0x33() { SWAP_register(E); }
void Processor::cb_opcode0x34() { SWAP_register(H); }
void Processor::cb_opcode0x35() { SWAP_register(L); }
void Processor::cb_opcode0x36() { SWAP_address(HL); }

#pragma endregion


/*	Rotate/Shift opcodes
*/
#pragma region

void Processor::opcode0x07() { RL_no_carry(A); }
void Processor::opcode0x0f() { RR_no_carry(A); }
void Processor::opcode0x17() { RL_carry(A); }
void Processor::opcode0x1f() { RR_carry(A); }

void Processor::cb_opcode0x11() { RL_carry(C); }

#pragma endregion


/*	Bit opcodes
*/
#pragma region

void Processor::cb_opcode0x7c()
{
    if ((H.value() >> 7 & 1) == 0) {
        set(flags.zero);
    }
    else {
        reset(flags.zero);
    }
    reset(flags.subtract);
    set(flags.half_carry);
}

#pragma endregion


/* Jump opcodes
*/
#pragma region

void Processor::opcode0x20()
{
    i8 jump = fetch_byte();
    if (!flags.zero) {
        PC.add(jump);
    }
}

#pragma endregion

/* Call/Return opcodes
*/
#pragma region

void Processor::opcode0xcd()
{
    PUSH_register(PC);
    LD_immediate(PC);
}

void Processor::opcode0xc9() { RET(); }
void Processor::opcode0xc0() { if (!flags.zero) RET(); }
void Processor::opcode0xc8() { if (flags.zero) RET(); }
void Processor::opcode0xd0() { if (!flags.carry) RET(); }
void Processor::opcode0xd8() { if (flags.carry) RET(); }

#pragma endregion




