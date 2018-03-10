#include "processor.h"
#include <iostream>

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
