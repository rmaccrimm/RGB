#ifndef DEBUG_MODE
#define DEBUG_MODE 0
#endif

#include "definitions.h"


namespace DEBUG
{
    u16 get_break_point();
    char debug_menu();
    void setup_stripe_pattern(u8 *memory);
    void setup_dot_pattern(u8 *memory);
    void setup_gradient_tile(u8 *memory);
    void print_tile_map(u8 *memory, bool map);

}
