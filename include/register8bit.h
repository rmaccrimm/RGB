#ifndef REGISTER8BIT_H
#define REGISTER8BIT_H

#include "definitions.h"

class Register8bit
{
public:
	Register8bit();
	
	void set(u8 x);
	u8 value();
	
	void inc();
	void dec();
	void add(u8 x);
	void sub(u8 x);

private:
	u8 byte;
};

#endif
