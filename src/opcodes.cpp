#include "processor.h"
#include <cassert>
#include <iostream>

void Processor::execute(u8 instr)
{
	assert(instr < 0x100);
	(this->*opcodes.at(instr))();
}

void Processor::cb_execute(u8 instr)
{
	assert(instr < 0x100);
	(this->*cb_opcodes.at(instr))();
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

void Processor::opcode0x3c() { A.increment(); }
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

void Processor::opcode0xc5() { PUSH_register(BC); }

void Processor::opcode0xcd()
{
	PUSH_register(PC);
	LD_immediate(PC);
}

void Processor::opcode0xe0() { memory[0xff00 + fetch_byte()] = A.value(); }
void Processor::opcode0xe2() { memory[0xff00 + C.value()] = A.value(); }

