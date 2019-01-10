#ifndef TILE_H
#define TILE_H

#include "definitions.h"
#include <vector>
#include <utility>

class Tile
{
public:
    Tile();
    std::vector<std::vector<u8>> lines;
    void write(int index, u8 byte);
};

class TileMapEntry
{
public:
    TileMapEntry(Tile *table_base);

    void update(u8 tile_index);

    Tile* get_tile(bool signed_addressing);

    u8 index;

    std::pair<Tile*, Tile*> tile_ptrs;

    Tile *const tile_table;
};

#endif