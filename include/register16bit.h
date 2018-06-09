#ifndef REGISTER16BIT_H
#define REGISTER16BIT_H

#include <memory>
#include "definitions.h"
#include "register8bit.h"

/* 16-bit registers can be used as a standalone register or as a reference
   to a pair of existing 8-bit registers. 
*/
class Register16bit

{
public:
    Register16bit();
    Register16bit(Register8bit *high, Register8bit *low);
    ~Register16bit();
    
    void set_low(u8 x);
    void set_high(u8 x);
    void set(u16 x);
    
    u16 value() const;
    u8 value_high() const;
    u8 value_low() const;

    void increment();
    void decrement();
    void add(u16 x);
    void sub(u16 x);

private:
    const bool referencing;
    Register8bit * const rlow;
    Register8bit * const rhigh;

};

#endif
