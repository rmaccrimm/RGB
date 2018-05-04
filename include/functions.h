#include "definitions.h"
#include <vector>

std::vector<u8> read_rom(size_t &file_size, const char *path);

void set(bool &b);

void reset(bool &b);

// Check for carry from bit 3 to bit 4 in a + b
bool half_carry_add(u16 a, u16 b);

// Check for borrow from bit 4 to 3 in a - b 
bool half_carry_sub(u16 a, u16 b);

// Check for carry from bit 7 to bit 8 in a + b
bool full_carry_add(u16 a, u16 b);

// Check for borrow from bit 8 to 7 in a - b 
bool full_carry_sub(u16 a, u16 b);

