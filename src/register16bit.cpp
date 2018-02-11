#include "register16bit.h"

Register16bit::Register16bit():
	referencing(false), r0(new Register8bit), r1(new Register8bit) {}

Register16bit::Register16bit(Register8bit *high, Register8bit *low):
	referencing(true), r0(low), r1(high) {}

Register16bit::~Register16bit()
{
	if (!referencing) {
		delete r0;
		delete r1;
	}
}

void Register16bit::set(u16 x)
{
	r1->set(x >> 8);
	r0->set(x & 0xff);
}

u16 Register16bit::value()
{
	return r1->value() << 8 | r0->value();
}

void Register16bit::set_low(u8 x) { r0->set(x); }

void Register16bit::set_high(u8 x) { r1->set(x); }

void Register16bit::increment() { set(value() + 1); }

void Register16bit::decrement() { set(value() - 1); }

void Register16bit::add(u16 x) { set(value() + x); }
