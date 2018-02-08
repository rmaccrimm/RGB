#ifndef REGISTER16BIT_H
#define REGISTER16BIT_H

#include <memory>
#include "definitions.h"
#include "register8bit.h"

class Register16bit
{
public:
	Register16bit();
	Register16bit(Register8bit *high, Register8bit *low);
	~Register16bit();
	
	void set_low(u8 x);
	void set_high(u8 x);
	void set(u16 x);
	
	u16 value();

	void inc();
	void dec();
	void add(u16 x);

private:
	Register8bit * const r0;
	Register8bit * const r1;
	const bool referencing;
};

#endif
