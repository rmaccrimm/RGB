#include "register8bit.h"

Register8bit::Register8bit(): byte(0) {}

void Register8bit::set(u8 x) { byte = x; }

u8 Register8bit::value() { return byte; }

void Register8bit::inc(){ byte++; }

void Register8bit::dec(){ byte--; }

void Register8bit::add(u8 x) { byte += x; }
