#include "processor.h"
#include <cassert>
#include <iostream>

bool Processor::execute(u8 instr, bool cb)
{
	assert(instr < 0x100);
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

/*bool Processor::cb_execute(u8 instr)
{
	assert(instr < 0x100);
	if (cb_opcodes[instr] == nullptr)
		return false;
	(this->*cb_opcodes[instr])();
	return true;
	}*/

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

bool Processor::check_half_carry(int a, int b)
{
	return (((a & 0xf) + (b & 0xf)) & 0x10) == 0x10;
}

void Processor::INC_register(Register8bit &reg)
{
	
	reg.increment();
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

// TODO - flags need to be set
void Processor::opcode0x3c() {
	A.increment();
}
void Processor::opcode0x04() { B.increment(); }
void Processor::opcode0x0c() { C.increment(); }
void Processor::opcode0x14() { D.increment(); }
void Processor::opcode0x1c() { E.increment(); }
void Processor::opcode0x24() { H.increment(); }
void Processor::opcode0x2c() { L.increment(); }
void Processor::opcode0x34() { memory[HL.value()]++; }

void Processor::opcode0x06() { LD_immediate(B); }
void Processor::opcode0x0e() { LD_immediate(C); }
void Processor::opcode0x3e() { LD_immediate(A); }

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
