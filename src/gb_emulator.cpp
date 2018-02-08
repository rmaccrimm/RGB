#include <iostream>
#include "definitions.h"
#include "register16bit.h"
#include "register8bit.h"
#include "processor.h"
using namespace std;

int main()
{
	Processor gb_cpu;
	gb_cpu.execute(0);
	gb_cpu.execute(1);
	return 0;
}
