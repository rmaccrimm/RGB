#include "register16bit.h"

Register16bit::Register16bit():
	referencing(false), rlow(new Register8bit), rhigh(new Register8bit) {}

Register16bit::Register16bit(Register8bit *high, Register8bit *low):
	referencing(true), rlow(low), rhigh(high) {}

Register16bit::~Register16bit()
{
	if (!referencing) {
		delete rlow;
		delete rhigh;
	}
}

void Register16bit::set(u16 x)
{
	rhigh->set(x >> 8);
	rlow->set(x & 0xff);
}

u16 Register16bit::value() const
{
	return rhigh->value() << 8 | rlow->value();
}

u8 Register16bit::value_low() const { return rlow->value(); }

u8 Register16bit::value_high() const { return rhigh->value(); }

void Register16bit::set_low(u8 x) { rlow->set(x); }

void Register16bit::set_high(u8 x) { rhigh->set(x); }

void Register16bit::increment() { set(value() + 1); }

void Register16bit::decrement() { set(value() - 1); }

void Register16bit::add(i16 x) { set(value() + x); }
