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
	
	u16 value() const;
	u8 value_high() const;
	u8 value_low() const;

	void increment();
	void decrement();
	void add(i16 x);

private:
	const bool referencing;
	Register8bit * const rlow;
	Register8bit * const rhigh;

};

#endif
