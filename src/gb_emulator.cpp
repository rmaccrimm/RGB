#include <iostream>
#include "definitions.h"
#include "register16bit.h"
#include "register8bit.h"
#include "processor.h"
using namespace std;

int main()
{
	Register8bit A;
	Register8bit F;
	Register16bit AF(&A, &F);
	Register16bit SP;
	SP.set(0x81);
	A.set(0xe3);
	F.set(0xff);
	cout << (int)AF.value() << ' ' << (int)SP.value() << endl;
	AF.set(0xfe99);
	cout << (int)A.value() << ' ' << (int)F.value() << endl;
	A.add(0x02);
	F.dec();
	A.inc();
	cout << (int)A.value() << ' ' << (int)F.value() << endl;
	SP.set(0xffff);
	SP.inc();
	cout << (int)SP.value() << endl;
	AF.set(0x00ff);
	AF.inc();
	cout << (int)A.value() << ' ' << (int)F.value() << endl;
	
	u8 u = 0x3e;
	Register8bit R;
	R.set(u);
	i8 i = -20;
	R.add(i);
	u8 n = u + i;
	cout << (int)R.value() << endl;
	Processor gb_cpu;
	return 0;
}
