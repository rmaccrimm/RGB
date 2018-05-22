#include "definitions.h"
#include <vector>

void load_rom(u8 memory[], const char *path);

void load_rom(u8 memory[], u8 data[], size_t size);

// Check for carry from bit 3 to bit 4 in a + b
bool half_carry_add(u16 a, u16 b);

// Check for borrow from bit 4 to 3 in a - b 
bool half_carry_sub(u16 a, u16 b);

// Check for carry from bit 7 to bit 8 in a + b
bool full_carry_add(u16 a, u16 b);

// Check for borrow from bit 8 to 7 in a - b 
bool full_carry_sub(u16 a, u16 b);

int tile_coord_to_map_index(int x, int y);

int tile_coord_to_pixel_index(int x, int y);