#include <cstdint>

typedef uint8_t u8;
typedef uint16_t u16;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;

union reg16 
{
    u16 val;
    struct {
        u8 low;
        u8 high;
    };
};
