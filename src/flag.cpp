#include "flag.h"

Flag::Flag(): f(false) {}

void Flag::is_set() { return f; }

void Flag::set() { f = true; }

void Flag::reset() { f = false; }
