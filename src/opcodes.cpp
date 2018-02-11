#include "processor.h"
#include <iostream>

void Processor::opcode0x00()
{
	std::cout << "Hello";
}

void Processor::opcode0x01()
{
	std::cout << " World." << std::endl;
}

void Processor::opcode0x31()
{
	PC.increment();
	SP.set_high(fetch_byte());
	PC.increment();
	SP.set_low(fetch_byte());
}

void Processor::opcode0xaf()
{
	A.set(0); // XOR with self
	zero_flag.set();
	subtract_flag.reset();
	half_carry_flag.reset();
	carry_flag.reset();
}
