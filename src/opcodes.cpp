#include "processor.h"
#include <cassert>
#include <iostream>

bool Processor::execute(u8 instr, bool cb)
{
	opfunc *op;
	if (cb) 
		op = &cb_opcodes[0];
	else 
		op = &opcodes[0];
	if (op[instr] == nullptr)
		return false;
	(this->*op[instr])();
	return true;
}

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
	SP.decrement();
	return data;
}

bool Processor::half_carry_add(int a, int b)
{
	return (((a & 0xf) + (b & 0xf)) & 0x10) == 0x10;
}

bool Processor::half_carry_sub(int a, int b)
{
	return ((a - (b & 0xff)) & 0x10) == 0;
}

void Processor::INC_register(Register8bit &reg) 
{
	half_carry_flag = half_carry_add(reg.value(), 1);
	subtract_flag.reset();
	reg.increment();
	zero_flag = (reg.value() == 0);
}

void Processor::INC_address(Register16bit const &reg)
{	
	half_carry_flag = half_carry_add(memory[reg.value()], 1);
	subtract_flag.reset();
	memory[reg.value()]++;
	zero_flag = (memory[reg.value()] == 0);
}

void Processor::DEC_register(Register8bit &reg)
{
	half_carry_flag = half_carry_sub(reg.value(), 1);
	subtract_flag.set();
	reg.decrement();
	zero_flag = (reg.value() == 0);
}

void Processor::DEC_address(Register16bit const &reg)
{
	half_carry_flag = half_carry_sub(memory[reg.value()], 1);
	subtract_flag.set();
	memory[reg.value()]--;
	zero_flag = (memory[reg.value()] == 0);
}

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

void Processor::RL_carry(Register8bit &reg)
{
	int temp = reg.value() >> 7; // bit 7
	reg.set((reg.value() << 1) | carry_flag.is_set());
	carry_flag = temp;
	flag_reset(reg);
}

void Processor::RR_carry(Register8bit &reg)
{
	int temp = reg.value() & 1; // bit 0
	reg.set((reg.value() >> 1) | (carry_flag.is_set() << 7));
	carry_flag = temp;
	flag_reset(reg);
}

void Processor::RL_no_carry(Register8bit &reg)
{
	int temp = reg.value() >> 7; // bit 7
	reg.set((reg.value() << 1) | temp);
	carry_flag = temp;
	flag_reset(reg);
}

void Processor::RR_no_carry(Register8bit &reg)
{
	int temp = reg.value() & 1; // bit 0
	reg.set((reg.value() >> 1) | (temp << 7));
	carry_flag = temp;
	flag_reset(reg);
}

void Processor::flag_reset(Register8bit const &reg)
{
	if (reg.value() == 0)
		zero_flag.set();
	subtract_flag.reset();
	half_carry_flag.reset();	
}

void Processor::opcode0x05() {}

void Processor::opcode0x3c() { INC_register(A); }
void Processor::opcode0x04() { INC_register(B); }
void Processor::opcode0x0c() { INC_register(C); }
void Processor::opcode0x14() { INC_register(D); }
void Processor::opcode0x1c() { INC_register(E); }
void Processor::opcode0x24() { INC_register(H); }
void Processor::opcode0x2c() { INC_register(L); }
void Processor::opcode0x34() { INC_address(HL); }

void Processor::opcode0x06() { LD_immediate(B); }
void Processor::opcode0x0e() { LD_immediate(C); }
void Processor::opcode0x16() { LD_immediate(D); }
void Processor::opcode0x1e() { LD_immediate(E); }
void Processor::opcode0x26() { LD_immediate(H); }
void Processor::opcode0x2e() { LD_immediate(L); }
void Processor::opcode0x3e() { LD_immediate(A); }

void Processor::opcode0x7f() { LD_register(A, A); }
void Processor::opcode0x78() { LD_register(A, B); }
void Processor::opcode0x79() { LD_register(A, C); }
void Processor::opcode0x7a() { LD_register(A, D); }
void Processor::opcode0x7b() { LD_register(A, E); }
void Processor::opcode0x7c() { LD_register(A, H); }
void Processor::opcode0x7d() { LD_register(A, L); }
void Processor::opcode0x7e() { LD_address(A, HL); }

void Processor::opcode0x40() { LD_register(B, B); }
void Processor::opcode0x41() { LD_register(B, C); }
void Processor::opcode0x42() { LD_register(B, D); }
void Processor::opcode0x43() { LD_register(B, E); }
void Processor::opcode0x44() { LD_register(B, H); }
void Processor::opcode0x45() { LD_register(B, L); }
void Processor::opcode0x46() { LD_address(B, HL); }

void Processor::opcode0x48() { LD_register(C, B); }
void Processor::opcode0x49() { LD_register(C, C); }
void Processor::opcode0x4a() { LD_register(C, D); }
void Processor::opcode0x4b() { LD_register(C, E); }
void Processor::opcode0x4c() { LD_register(C, H); }
void Processor::opcode0x4d() { LD_register(C, L); }
void Processor::opcode0x4e() { LD_address(C, HL); }

void Processor::opcode0x50() { LD_register(D, B); }
void Processor::opcode0x51() { LD_register(D, C); }
void Processor::opcode0x52() { LD_register(D, D); }
void Processor::opcode0x53() { LD_register(D, E); }
void Processor::opcode0x54() { LD_register(D, H); }
void Processor::opcode0x55() { LD_register(D, L); }
void Processor::opcode0x56() { LD_address(D, HL); }

void Processor::opcode0x58() { LD_register(E, B); }
void Processor::opcode0x59() { LD_register(E, C); }
void Processor::opcode0x5a() { LD_register(E, D); }
void Processor::opcode0x5b() { LD_register(E, E); }
void Processor::opcode0x5c() { LD_register(E, H); }
void Processor::opcode0x5d() { LD_register(E, L); }
void Processor::opcode0x5e() { LD_address(E, HL); }

void Processor::opcode0x60() { LD_register(H, B); }
void Processor::opcode0x61() { LD_register(H, C); }
void Processor::opcode0x62() { LD_register(H, D); }
void Processor::opcode0x63() { LD_register(H, E); }
void Processor::opcode0x64() { LD_register(H, H); }
void Processor::opcode0x65() { LD_register(H, L); }
void Processor::opcode0x66() { LD_address(H, HL); }

void Processor::opcode0x68() { LD_register(L, B); }
void Processor::opcode0x69() { LD_register(L, C); }
void Processor::opcode0x6a() { LD_register(L, D); }
void Processor::opcode0x6b() { LD_register(L, E); }
void Processor::opcode0x6c() { LD_register(L, H); }
void Processor::opcode0x6d() { LD_register(L, L); }
void Processor::opcode0x6e() { LD_address(L, HL); }

void Processor::opcode0x70() { LD_address(HL, B); }
void Processor::opcode0x71() { LD_address(HL, C); }
void Processor::opcode0x72() { LD_address(HL, D); }
void Processor::opcode0x73() { LD_address(HL, E); }
void Processor::opcode0x74() { LD_address(HL, H); }
void Processor::opcode0x75() { LD_address(HL, L); }
void Processor::opcode0x36() { memory[HL.value()] = fetch_byte(); }

void Processor::opcode0x20()
{
	i8 jump = fetch_byte();
	if (!zero_flag.is_set()) {
		PC.add(jump);
	}
}

void Processor::opcode0x1a() { LD_address(A, DE); }

void Processor::opcode0x11() { LD_immediate(DE); }
void Processor::opcode0x21() { LD_immediate(HL); }
void Processor::opcode0x31() { LD_immediate(SP); }

void Processor::opcode0x07() { RL_no_carry(A); }
void Processor::opcode0x0f() { RR_no_carry(A); }
void Processor::opcode0x17() { RL_carry(A); }
void Processor::opcode0x1f() { RR_carry(A); }

void Processor::opcode0x32()
{
	memory[HL.value()] = A.value();
	HL.decrement();
}

void Processor::opcode0x4f() { LD_register(C, A); }

void Processor::opcode0x77() { memory[HL.value()] = A.value(); }

void Processor::opcode0xaf()
{
	A.set(0); // XOR with self
	zero_flag.set();
	subtract_flag.reset();
	half_carry_flag.reset();
	carry_flag.reset();
}

void Processor::opcode0xf1() { POP_register(AF); }
void Processor::opcode0xc1() { POP_register(BC); }
void Processor::opcode0xd1() { POP_register(DE); }
void Processor::opcode0xe1() { POP_register(HL); }

void Processor::opcode0xf5() { PUSH_register(AF); }
void Processor::opcode0xc5() { PUSH_register(BC); }
void Processor::opcode0xd5() { PUSH_register(DE); }
void Processor::opcode0xe5() { PUSH_register(HL); }



void Processor::opcode0xcd()
{
	PUSH_register(PC);
	LD_immediate(PC);
}

void Processor::opcode0xe0() { memory[0xff00 + fetch_byte()] = A.value(); }
void Processor::opcode0xe2() { memory[0xff00 + C.value()] = A.value(); }

void Processor::cb_opcode0x11() { RL_carry(C); }

void Processor::cb_opcode0x7c()
{
	if ((H.value() >> 7 & 1) == 0) {
		zero_flag.set();
	}
	else {
		zero_flag.reset();
	}
	subtract_flag.reset();
	half_carry_flag.reset();
}
