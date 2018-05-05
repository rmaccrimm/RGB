#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <map>
#include "definitions.h"
#include "register8bit.h"
#include "register16bit.h"
#include "functions.h"
#include "memory.h"

class Processor
{
    // opfunc type defined as shorthand for void member functions 
    typedef void (Processor::*OpFunc)(void);
    
    struct Flags {
        bool zero;       // Z
        bool subtract;   // N
        bool carry;      // H
        bool half_carry; // C
    };	

public:
    Processor(u8 *mem = nullptr);
    
    bool step();
    void set_memory(u8 *mem);
    void print_register_values();
    void map_to_memory(u8 program[], u16 size, u16 offset);

//private:
    Register8bit A;
    Register8bit F;
    Register8bit B;
    Register8bit C;
    Register8bit D;
    Register8bit E;
    Register8bit H;
    Register8bit L;
    
    Register16bit AF;
    Register16bit BC;
    Register16bit DE;	
    Register16bit HL;
    Register16bit SP;
    Register16bit PC;

    //Memory *memory;

    Flags flags;
    OpFunc opcodes[0x100];	
    OpFunc cb_opcodes[0x100];
    u8 *memory; // 16 kB memory

    u8 fetch_byte();

    // Return false if  unimplemented
    bool execute(u8 instr, bool cb);

    void flag_set(Register8bit const &reg);
    void set_add_flags(u16 a, u16 b);
    void set_sub_flags(u16 a, u16 b);
    void flag_reset(Register8bit const &reg);
    void set_and_flags(u8 val);
    void set_or_flags(u8 val);
    
    void stack_push(u8 data);
    u8 stack_pop();

    void LD_immediate(Register8bit &reg);
    void LD_immediate(Register16bit &reg);
    void LD_register(Register8bit &dest, Register8bit const &src);
    void LD_register(Register16bit &dest, Register16bit const &src);
    void LD_address(Register8bit &dest, Register16bit const &src);
    void LD_address(Register16bit const &dest, Register8bit const &src);

    // The stack is at address 0xfffe backwards. High is pushed then low for 16 bit.
    void PUSH_register(Register8bit const &reg);
    void PUSH_register(Register16bit const &reg);
    void POP_register(Register8bit &reg);
    void POP_register(Register16bit &reg);

    // Carry argument used for ADC/SBC 
    void ADD_register(Register8bit &dest, Register8bit const &src, bool carry = false);
    void ADD_register(Register16bit &dest, Register16bit const &src);
    void ADD_immediate(Register8bit &reg, bool carry = false);
    void ADD_immediate(Register16bit &reg);
    void ADD_address(Register8bit &dest, Register16bit const &src, bool carry = false);

    void SUB_register(Register8bit &dest, Register8bit const &src, bool carry = false);
    void SUB_immediate(Register8bit &reg, bool carry = false);
    void SUB_address(Register8bit &dest, Register16bit const &src, bool carry = false);

    void INC_register(Register8bit &reg);
    void INC_register(Register16bit &reg);
    void INC_address(Register16bit const &reg); 

    void DEC_register(Register8bit &reg);
    void DEC_register(Register16bit &reg);
    void DEC_address(Register16bit const &reg);

    void AND_register(Register8bit &dest, Register8bit &src);
    void AND_immediate(Register8bit &reg);
    void AND_address(Register8bit &dest, Register16bit &src);

    void OR_register(Register8bit &dest, Register8bit &src);
    void OR_immediate(Register8bit &reg);
    void OR_address(Register8bit &dest, Register16bit &src);

    void XOR_register(Register8bit &dest, Register8bit &src);
    void XOR_immediate(Register8bit &reg);
    void XOR_address(Register8bit &dest, Register16bit &src);

    void CP_register(Register8bit &dest, Register8bit &src);
    void CP_immediate(Register8bit &reg);
    void CP_address(Register8bit &dest, Register16bit &src);

    void RL_carry(Register8bit &reg);
    void RR_carry(Register8bit &reg);
    void RL_no_carry(Register8bit &reg);
    void RR_no_carry(Register8bit &reg);

    void SR_arithmetic(Register8bit &reg);
    void SL_arithmetic(Register8bit &reg);
    void SR_logical(Register8bit &reg);
    void SL_logical(Register8bit &reg);

    void BIT(Register8bit &reg, u8 bit);
    void SET(Register8bit &reg, u8 bit);
    void RES(Register8bit &reg, u8 bit);

    void opcode0x00();	void opcode0x01();	void opcode0x02();	void opcode0x03();
    void opcode0x04(); 	void opcode0x05();	void opcode0x06(); 	void opcode0x07(); 
    void opcode0x08(); 	void opcode0x09();	void opcode0x0a();	void opcode0x0b();
    void opcode0x0c(); 	void opcode0x0d();	void opcode0x0e(); 	void opcode0x0f();
    void opcode0x10();	void opcode0x11(); 	void opcode0x12();	void opcode0x13();
    void opcode0x14(); 	void opcode0x15();	void opcode0x16();	void opcode0x17(); 
    void opcode0x18();	void opcode0x19();	void opcode0x1a(); 	void opcode0x1b();
    void opcode0x1c(); 	void opcode0x1d();	void opcode0x1e();	void opcode0x1f(); 
    void opcode0x20(); 	void opcode0x21(); 	void opcode0x22();	void opcode0x23();
    void opcode0x24(); 	void opcode0x25();	void opcode0x26();	void opcode0x27();
    void opcode0x28();	void opcode0x29();	void opcode0x2a();	void opcode0x2b();
    void opcode0x2c(); 	void opcode0x2d();	void opcode0x2e();	void opcode0x2f();
    void opcode0x30();	void opcode0x31(); 	void opcode0x32(); 	void opcode0x33();
    void opcode0x34(); 	void opcode0x35();	void opcode0x36();	void opcode0x37();
    void opcode0x38();	void opcode0x39();	void opcode0x3a();	void opcode0x3b();
    void opcode0x3c(); 	void opcode0x3d();	void opcode0x3e(); 	void opcode0x3f();
    void opcode0x40();	void opcode0x41();	void opcode0x42();	void opcode0x43();
    void opcode0x44();	void opcode0x45();	void opcode0x46();	void opcode0x47();
    void opcode0x48();	void opcode0x49();	void opcode0x4a();	void opcode0x4b();
    void opcode0x4c();	void opcode0x4d();	void opcode0x4e();	void opcode0x4f(); 
    void opcode0x50();	void opcode0x51();	void opcode0x52();	void opcode0x53();
    void opcode0x54();	void opcode0x55();	void opcode0x56();	void opcode0x57();
    void opcode0x58();	void opcode0x59();	void opcode0x5a();	void opcode0x5b();
    void opcode0x5c();	void opcode0x5d();	void opcode0x5e();	void opcode0x5f();
    void opcode0x60();	void opcode0x61();	void opcode0x62();	void opcode0x63();
    void opcode0x64();	void opcode0x65();	void opcode0x66();	void opcode0x67();
    void opcode0x68();	void opcode0x69();	void opcode0x6a();	void opcode0x6b();
    void opcode0x6c();	void opcode0x6d();	void opcode0x6e();	void opcode0x6f();
    void opcode0x70();	void opcode0x71();	void opcode0x72();	void opcode0x73();
    void opcode0x74();	void opcode0x75();	void opcode0x76();	void opcode0x77(); 
    void opcode0x78();	void opcode0x79();	void opcode0x7a();	void opcode0x7b();
    void opcode0x7c();	void opcode0x7d();	void opcode0x7e();	void opcode0x7f();
    void opcode0x80();	void opcode0x81();	void opcode0x82();	void opcode0x83();
    void opcode0x84();	void opcode0x85();	void opcode0x86();	void opcode0x87();
    void opcode0x88();	void opcode0x89();	void opcode0x8a();	void opcode0x8b();
    void opcode0x8c();	void opcode0x8d();	void opcode0x8e();	void opcode0x8f();
    void opcode0x90();	void opcode0x91();	void opcode0x92();	void opcode0x93();
    void opcode0x94();	void opcode0x95();	void opcode0x96();	void opcode0x97();
    void opcode0x98();	void opcode0x99();	void opcode0x9a();	void opcode0x9b();
    void opcode0x9c();	void opcode0x9d();	void opcode0x9e();	void opcode0x9f();
    void opcode0xa0();	void opcode0xa1();	void opcode0xa2();	void opcode0xa3();
    void opcode0xa4();	void opcode0xa5();	void opcode0xa6();	void opcode0xa7();
    void opcode0xa8();	void opcode0xa9();	void opcode0xaa();	void opcode0xab();
    void opcode0xac();	void opcode0xad();	void opcode0xae();	void opcode0xaf(); 
    void opcode0xb0();	void opcode0xb1();	void opcode0xb2();	void opcode0xb3();
    void opcode0xb4();	void opcode0xb5();	void opcode0xb6();	void opcode0xb7();
    void opcode0xb8();	void opcode0xb9();	void opcode0xba();	void opcode0xbb();
    void opcode0xbc();	void opcode0xbd();	void opcode0xbe();	void opcode0xbf();
    void opcode0xc0();	void opcode0xc1(); 	void opcode0xc2();	void opcode0xc3();
    void opcode0xc4();	void opcode0xc5(); 	void opcode0xc6();	void opcode0xc7();
    void opcode0xc8();	void opcode0xc9();	void opcode0xca();	void opcode0xcb();
    void opcode0xcc();	void opcode0xcd(); 	void opcode0xce();	void opcode0xcf();
    void opcode0xd0();	void opcode0xd1(); 	void opcode0xd2();	void opcode0xd3(){}
    void opcode0xd4();	void opcode0xd5(); 	void opcode0xd6();	void opcode0xd7();	
    void opcode0xd8();	void opcode0xd9();	void opcode0xda();	void opcode0xdb(){}
    void opcode0xdc();	void opcode0xdd(){}	void opcode0xde();	void opcode0xdf();
    void opcode0xe0(); 	void opcode0xe1(); 	void opcode0xe2();  void opcode0xe3(){}
    void opcode0xe4(){}	void opcode0xe5(); 	void opcode0xe6();	void opcode0xe7();	
    void opcode0xe8();	void opcode0xe9();	void opcode0xea();	void opcode0xeb(){}
    void opcode0xec(){}	void opcode0xed(){}	void opcode0xee();	void opcode0xef();
    void opcode0xf0();	void opcode0xf1(); 	void opcode0xf2();	void opcode0xf3();
    void opcode0xf4(){}	void opcode0xf5(); 	void opcode0xf6();	void opcode0xf7();	
    void opcode0xf8();	void opcode0xf9();	void opcode0xfa();	void opcode0xfb();						
    void opcode0xfc(){}	void opcode0xfd(){}	void opcode0xfe();	void opcode0xff();	
    

    void cb_opcode0x11(); 
    void cb_opcode0x7c(); 

};

#endif
