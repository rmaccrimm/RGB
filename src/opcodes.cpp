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
