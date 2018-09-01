#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <map>
#include "definitions.h"
#include "register8bit.h"
#include "register16bit.h"
#include "functions.h"
#include "mmu.h"
#include "operations.h"

class Processor
{
public:
    Processor(Memory *mem);
    void init_state();
    int step(bool print = false);
    void set_flags(u8 mask, bool b);
    void update_timer(int cycles);
    void process_interrupts();
    void execute(u8 instr);
    void cb_execute(u8 instr);

    u8 fetch_byte();
    u16 fetch_word();

    bool zero_flag();         // Z
    bool subtract_flag();     // N
    bool half_carry_flag();   // H
    bool carry_flag();        // C

    bool interrupt_pending();

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

    Memory *memory; 

    bool IME_flag;
    bool halted;
    int ei_count;
    int clock_count;
    int timer_count;

    static const int instr_cycles[256];
    static const int cb_instr_cycles[256];
    static const int timer_cycles[4];
    static const u16 interrupt_addr[5];

    static const int div_cycles = 256;

    // F register flags
    static const u8 ZERO = 1 << 7;       
    static const u8 SUBTRACT = 1 << 6;   
    static const u8 HALF_CARRY = 1 << 5; 
    static const u8 CARRY = 1 << 4;      
};    

#endif
