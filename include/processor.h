#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <memory>
#include "definitions.h"
#include "register8bit.h"
#include "register16bit.h"

struct Flag
{
	bool zero;
	bool subtract;
	bool half_carry;
	bool carry;
};

class Processor
{
public:
	Processor(); 
	
private:
	Register8bit A;
	Register8bit F;
	Register16bit AF;
	
	Register8bit B;
	Register8bit C;
	Register16bit BC;
	
	Register8bit D;
	Register8bit E;
	Register16bit DE;
	
	Register8bit H;
	Register8bit L;
	Register16bit HL;
	
	Register16bit SP;
	Register16bit PC;

	Flag flag;
	u8 memory[0x10000]; // 16 kB memory

	void opcode0x00();
	void opcode0xff();
	void opcode0xcb00();
	void opcode0xcbff();
};

#endif
