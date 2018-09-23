#include "definitions.h"
#include <vector>
#include <string>

namespace utils
{
    void load_rom(u8 memory[], size_t offset, const char *path);

    void load_rom(u8 memory[], size_t start, size_t offset, const char *path);

    void load_rom(u8 memory[], u8 data[], size_t size);

    // Check for carry from bit 3 to bit 4 in a + b
    bool half_carry_add(u16 a, u16 b);

    // Check for borrow from bit 4 to 3 in a - b 
    bool half_carry_sub(u16 a, u16 b);

    // Check for carry from bit 7 to bit 8 in a + b
    bool full_carry_add(u16 a, u16 b);

    // Check for borrow from bit 8 to 7 in a - b 
    bool full_carry_sub(u16 a, u16 b);

    // swap high and low 4 bits
    u8 swap(u8 n);

    u8 set(u8 x, int bit);

    u8 reset(u8 x, int bit);

    void to_lower(std::string &s);

    void to_upper(std::string &s);

    bool bit(int x, int bit);
};