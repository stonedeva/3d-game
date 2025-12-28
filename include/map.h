#ifndef _MAP_H_
#define _MAP_H_

#define MAP_WIDTH 24
#define MAP_HEIGHT 24

typedef enum {
    TILE_EMPTY = 0,
    TILE_DARK_STONE,
    TILE_LIGHT_STONE,
    TILE_LIGHT_BREAKSTONE0,
    TILE_LIGHT_BREAKSTONE1,
    TILE_LIGHT_BREAKSTONE2,
    TILE_LIGHT_BREAKSTONE3,
    TILE_LIGHT_BREAKSTONE4,
    TILE_DOOR,
    TILE_TNT,
} Tile;

extern Tile map[MAP_WIDTH][MAP_HEIGHT];


void map_load_from_file(char* file_path);
void map_break_block(int map_x, int map_y, Tile end_tile);
void map_explode_block(int map_x, int map_y);

#endif // _MAP_H_
