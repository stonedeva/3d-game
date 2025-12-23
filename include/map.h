#ifndef _MAP_H_
#define _MAP_H_

#define MAP_WIDTH 24
#define MAP_HEIGHT 24

typedef enum {
    TILE_EMPTY = 0,
    TILE_DARK_STONE,
    TILE_LIGHT_STONE,
    TILE_BREAKABLE_STONE,
    TILE_GATE
} Tile;

extern Tile map[MAP_WIDTH][MAP_HEIGHT];

#endif // _MAP_H_
