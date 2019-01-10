#include "tile.h"
#include "util.h"
#include <cassert>

Tile::Tile()
{
    lines.resize(8);
    for (auto &x: lines) {
        x.resize(8, 0);
    }
}

void Tile::write(int index, u8 byte)
{   
    // index is a value from 0 - 16, with 2 bytes determining each line
    assert(index < 16);
    assert(index >= 0);
    // even bytes contain lower bit of pixel color, odd contain upper bit    
    bool bit = utils::even(index);
    for (int i = 0; i < 8; i++) {
        u8 prev = lines[index / 2][i];
        lines[index / 2][i] = utils::set_cond(prev, bit, utils::bit(byte, i));
    }
}

TileMapEntry::TileMapEntry(Tile *const table_base): 
    tile_table{table_base}, tile_ptrs{table_base, table_base}, index{0} {}

void TileMapEntry::update(u8 tile_index)
{
    index = tile_index;
    tile_ptrs.first = tile_table + index;
    tile_ptrs.second = tile_table + 256 + (i8)index;
}

Tile* TileMapEntry::get_tile(bool signed_addressing)
{
    return signed_addressing ? tile_ptrs.second : tile_ptrs.first;
}