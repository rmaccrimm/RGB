#include "processor.h"
#include <iostream>
#include <cstring>
#include <cassert>

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
	return memory[PC.value()];
}

void Processor::run()
{
	for (int i = 0; i < 256; i++) {
		u8 instr = fetch_byte();
		
		if (instr == 0xcb) {
			PC.increment();
			try {
				cb_execute(fetch_byte());
			} catch(std::out_of_range exc) {
				std::cout << "Unimplemented instruction: 0xcb " << std::hex
						  << (int)instr << std::endl;
				return;
			}
		}
		else {
			try {
				execute(instr);
			} catch(std::out_of_range exc) {
				std::cout << "Unimplemented instruction: " << std::hex
						  << (int)instr << std::endl;
				return;
			}
		}
		PC.increment();
	}
}

void Processor::map_to_memory(u8 program[], u16 size, u16 offset)
{
	std::memcpy(&memory[offset], program, size);
}

Processor::Processor():
	A(), F(), B(), C(), D(), E(), H(), L(),
	AF(&A, &F), BC(&B, &C),	DE(&D, &F), HL(&H, &L)
{
	opcodes[0x00] = &Processor::opcode0x00;
	opcodes[0x01] = &Processor::opcode0x01;
	opcodes[0x31] = &Processor::opcode0x31;
	opcodes[0x31] = &Processor::opcode0x31;
	opcodes[0xaf] = &Processor::opcode0xaf;
}
