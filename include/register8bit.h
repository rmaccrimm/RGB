#ifndef REGISTER8BIT_H
#define REGISTER8BIT_H

#include "definitions.h"

class Register8bit
{
public:
    // Read/Write register initialized to 0
    Register8bit();

    // Unused bits will always store 1
    Register8bit(u8 unused);

    // Read-only bits will be modified by set but not write
    Register8bit(u8 unused, u8 read_only);
    
    // Directly set contents of register
    void set(u8 x);

    // Set contents, masking out read-only bits
    void write(u8 x);

    // Get contents of register
    u8 value() const;
    
    void increment();

    void decrement();

    void add(u8 x);

    void sub(u8 x);

private:
    u8 byte;
    u8 write_mask;
    u8 read_mask;
};

#endif
