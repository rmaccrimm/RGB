#ifndef REGISTER8BIT_H
#define REGISTER8BIT_H

#include "definitions.h"

class Register8bit
{
public:
    // Read/Write register initialized to 0
    Register8bit();

    // Mask out read-only bits
    Register8bit(u8 w_mask);
    
    // Directly set contents of register
    void set(u8 x);

    // Set contents, masking out read-only bits
    void write(u8 x);

    u8 value() const;
    
    void increment();

    void decrement();

    void add(u8 x);

    void sub(u8 x);

private:
    u8 byte;
    u8 write_mask;
};

#endif
