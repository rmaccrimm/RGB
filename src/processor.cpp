#include "processor.h"

Processor::Processor():
	A(), F(), B(), C(), D(), E(), H(), L(),
	AF(&A, &F), BC(&B, &C),	DE(&D, &F), HL(&H, &L)
{}
