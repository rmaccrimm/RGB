#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <map>
#include "definitions.h"
#include "register8bit.h"
#include "register16bit.h"
#include "functions.h"
#include "memory.h"
#include "operations.h"

class Processor
{
public:
    Processor(u8 *mem = nullptr);

    bool step(int break_point = 0);
    void print_registers();

    void execute(u8 instr);
    void cb_execute(u8 instr);
    void process_interrupts() {}

    u8 fetch_byte();
    u16 fetch_word();

    void set_flag(u8 mask, bool b);

    static const u8 ZERO = 1 << 7;       // Z
    static const u8 SUBTRACT = 1 << 6;   // N
    static const u8 HALF_CARRY = 1 << 5; // C
    static const u8 CARRY = 1 << 4;      // H

private:
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

    u8 *memory; 
    int clock_cycles;

    

    // For setting the correct bits of the flag register F
    /*void set(int b);
    void reset(int b);
    // Set if true, else reset
    void set_cond(int b, bool cond);
    bool is_set(int b);

    void flag_set(Register8bit const &reg);

    void set_add_flags(u16 a, u16 b);
    void set_sub_flags(u16 a, u16 b);

    void flag_reset(Register8bit const &reg);
    void flag_reset(Register16bit const &reg);

    void set_and_flags(u8 val);
    void set_or_flags(u8 val);
    
    void stack_push(u8 data);
    u8 stack_pop();*/
};

#endif
