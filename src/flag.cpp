#include "flag.h"
#include <cassert>

Flag::Flag(): f(false) {}

bool Flag::is_set() { return f; }

void Flag::set() { f = true; }

void Flag::reset() { f = false; }

void Flag::operator=(int val)
{
	assert(val == 0 || val == 1);
	f = val;
}
