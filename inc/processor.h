#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <map>
#include "definitions.h"
#include "flag.h"
#include "register8bit.h"
#include "register16bit.h"

class Processor
{
	/* The opfunc type defined for void member functions.
	   OpcodeMap makes it easier to execute a function given an opcode.
	*/  
	typedef void (Processor::*opfunc)(void);
	typedef std::map<u8, opfunc> OpcodeMap;
	
public:
	Processor();

	// Begins program execution from memory location 0 
	void run();
	
	// For debugging 
	void print_register_values();

	// Place size bytes of data in program into memory starting at offset 
	void map_to_memory(u8 program[], u16 size, u16 offset);
	
private:

	// Returns byte from memory that PC currently points to	and increment PC
	u8 fetch_byte();

	// Executes a non-prefixed instruction.
	void execute(u8 instr);

	// Execute a 0xCB-prefixed instruction.
	void cb_execute(u8 instr);

	Register8bit A;
	Register8bit F;
	Register8bit B;
	Register8bit C;
	Register8bit D;
	Register8bit E;
	Register8bit H;
	Register8bit L;

	/* 16-bit registers allow easier access to pairs of 8-bit registers. 
	   Setting these sets corresponding 8-bit registers. Order is High, Low
	*/
	Register16bit AF;
	Register16bit BC;
	Register16bit DE;	
	Register16bit HL;
	Register16bit SP;
	Register16bit PC;

	Flag zero_flag;        // Z
	Flag subtract_flag;    // N
	Flag half_carry_flag;  // H
	Flag carry_flag;       // C
	
	u8 memory[0x10000]; // 16 kB memory

	OpcodeMap opcodes;
	OpcodeMap cb_opcodes;

	void stack_push(u8 data);
	u8 stack_pop();

	// Load value of next byte in memory to 8 bit register
	void LD_immediate(Register8bit &reg);
	
	// Load value of next two bytes in memory to 16 bit register 
	void LD_immediate(Register16bit &reg);

	// Load byte from memory address stored in src into dest register
	void LD_address(Register8bit &dest, Register16bit const &src);

	// Load value of src register into dest register
	void LD_register(Register8bit &dest, Register8bit const &src);

	// Push value of register onto stack
	void PUSH_register(Register8bit const &reg);

	// Push values of register pair onto stack. High is pushed first, then low
	void PUSH_register(Register16bit const &reg);

	// Rotate contents of register left through carry 
	void RL_carry(Register8bit reg);

	// Rotate contents of register right through carry
	void RR_carry(Register8bit reg);
	
	void RL_no_carry(Register8bit reg);
	
	void RR_no_carry(Register8bit reg);

	void opcode0x00();
	void opcode0x01();
	void opcode0x02();
	void opcode0x03();
	void opcode0x04(); // INC B
	void opcode0x05();
	void opcode0x06(); // LD B, n
	void opcode0x07();
	void opcode0x08();
	void opcode0x09();
	void opcode0x0a();
	void opcode0x0b();
	void opcode0x0c(); // INC C
	void opcode0x0d();
	void opcode0x0e(); // LD C, n
	void opcode0x0f();
	void opcode0x10();
	void opcode0x11(); // LD DE, nn
	void opcode0x12();
	void opcode0x13();
	void opcode0x14(); // INC D
	void opcode0x15();
	void opcode0x16();
	void opcode0x17();
	void opcode0x18();
	void opcode0x19();
	void opcode0x1a(); // LD A, (DE)
	void opcode0x1b();
	void opcode0x1c(); // INC E
	void opcode0x1d();
	void opcode0x1e();
	void opcode0x1f();
	void opcode0x20(); // JR NZ
	void opcode0x21(); // LD HL, nn
	void opcode0x22();
	void opcode0x23();
	void opcode0x24(); // INC H
	void opcode0x25();
	void opcode0x26();
	void opcode0x27();
	void opcode0x28();
	void opcode0x29();
	void opcode0x2a();
	void opcode0x2b();
	void opcode0x2c(); // INC L
	void opcode0x2d();
	void opcode0x2e();
	void opcode0x2f();
	void opcode0x30();
	void opcode0x31(); // LD SP, nn
	void opcode0x32(); // LD (HL-), A
	void opcode0x33();
	void opcode0x34(); // INC (HL)
	void opcode0x35();
	void opcode0x36();
	void opcode0x37();
	void opcode0x38();
	void opcode0x39();
	void opcode0x3a();
	void opcode0x3b();
	void opcode0x3c(); // INC A
	void opcode0x3d();
	void opcode0x3e(); // LD A, n
	void opcode0x3f();
	void opcode0x40();
	void opcode0x41();
	void opcode0x42();
	void opcode0x43();
	void opcode0x44();
	void opcode0x45();
	void opcode0x46();
	void opcode0x47();
	void opcode0x48();
	void opcode0x49();
	void opcode0x4a();
	void opcode0x4b();
	void opcode0x4c();
	void opcode0x4d();
	void opcode0x4e();
	void opcode0x4f(); // LD C, A
	void opcode0x50();
	void opcode0x51();
	void opcode0x52();
	void opcode0x53();
	void opcode0x54();
	void opcode0x55();
	void opcode0x56();
	void opcode0x57();
	void opcode0x58();
	void opcode0x59();
	void opcode0x5a();
	void opcode0x5b();
	void opcode0x5c();
	void opcode0x5d();
	void opcode0x5e();
	void opcode0x5f();
	void opcode0x60();
	void opcode0x61();
	void opcode0x62();
	void opcode0x63();
	void opcode0x64();
	void opcode0x65();
	void opcode0x66();
	void opcode0x67();
	void opcode0x68();
	void opcode0x69();
	void opcode0x6a();
	void opcode0x6b();
	void opcode0x6c();
	void opcode0x6d();
	void opcode0x6e();
	void opcode0x6f();
	void opcode0x70();
	void opcode0x71();
	void opcode0x72();
	void opcode0x73();
	void opcode0x74();
	void opcode0x75();
	void opcode0x76();
	void opcode0x77(); // LD (HL), A
	void opcode0x78();
	void opcode0x79();
	void opcode0x7a();
	void opcode0x7b();
	void opcode0x7c();
	void opcode0x7d();
	void opcode0x7e();
	void opcode0x7f();
	void opcode0x80();
	void opcode0x81();
	void opcode0x82();
	void opcode0x83();
	void opcode0x84();
	void opcode0x85();
	void opcode0x86();
	void opcode0x87();
	void opcode0x88();
	void opcode0x89();
	void opcode0x8a();
	void opcode0x8b();
	void opcode0x8c();
	void opcode0x8d();
	void opcode0x8e();
	void opcode0x8f();
	void opcode0x90();
	void opcode0x91();
	void opcode0x92();
	void opcode0x93();
	void opcode0x94();
	void opcode0x95();
	void opcode0x96();
	void opcode0x97();
	void opcode0x98();
	void opcode0x99();
	void opcode0x9a();
	void opcode0x9b();
	void opcode0x9c();
	void opcode0x9d();
	void opcode0x9e();
	void opcode0x9f();
	void opcode0xa0();
	void opcode0xa1();
	void opcode0xa2();
	void opcode0xa3();
	void opcode0xa4();
	void opcode0xa5();
	void opcode0xa6();
	void opcode0xa7();
	void opcode0xa8();
	void opcode0xa9();
	void opcode0xaa();
	void opcode0xab();
	void opcode0xac();
	void opcode0xad();
	void opcode0xae();
	void opcode0xaf(); // XOR A, A
	void opcode0xb0();
	void opcode0xb1();
	void opcode0xb2();
	void opcode0xb3();
	void opcode0xb4();
	void opcode0xb5();
	void opcode0xb6();
	void opcode0xb7();
	void opcode0xb8();
	void opcode0xb9();
	void opcode0xba();
	void opcode0xbb();
	void opcode0xbc();
	void opcode0xbd();
	void opcode0xbe();
	void opcode0xbf();
	void opcode0xc0();
	void opcode0xc1();
	void opcode0xc2();
	void opcode0xc3();
	void opcode0xc4();
	void opcode0xc5(); // PUSH BC
	void opcode0xc6();
	void opcode0xc7();
	void opcode0xc8();
	void opcode0xc9();
	void opcode0xca();
	void opcode0xcb();
	void opcode0xcc();
	void opcode0xcd(); // CALL nn
	void opcode0xce();
	void opcode0xcf();
	void opcode0xd0();
	void opcode0xd1();
	void opcode0xd2();
	void opcode0xd4();
	void opcode0xd5();
	void opcode0xd6();
	void opcode0xd7();
	void opcode0xd8();
	void opcode0xd9();
	void opcode0xda();
	void opcode0xdc();
	void opcode0xde();
	void opcode0xdf();
	void opcode0xe0(); // LD ($FF00 + n), A
	void opcode0xe1();
	void opcode0xe2(); // LD ($FF00 + C), A
	void opcode0xe5();
	void opcode0xe6();
	void opcode0xe7();
	void opcode0xe8();
	void opcode0xe9();
	void opcode0xea();
	void opcode0xee();
	void opcode0xef();
	void opcode0xf0();
	void opcode0xf1();
	void opcode0xf2();
	void opcode0xf3();
	void opcode0xf5();
	void opcode0xf6();
	void opcode0xf7();
	void opcode0xf8();
	void opcode0xf9();
	void opcode0xfa();
	void opcode0xfb();
	void opcode0xfe();
	void opcode0xff();
	
	void opcode_cb0x11(); // RL C
	void cb_opcode0x7c(); // BIT 7, H

};

#endif
