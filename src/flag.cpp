#include "flag.h"

Flag::Flag(): f(false) {}

bool Flag::is_set() { return f; }

void Flag::set() { f = true; }

void Flag::reset() { f = false; }
