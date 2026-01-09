#ifndef _MAP_H_
#define _MAP_H_

#include "./player.h"

#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define GHOST_STONE_CAP 5

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
    TILE_ICE_GROUND,
    TILE_ICE_DARK_STONE,
    TILE_ICE_LIGHT_STONE,
    TILE_MAGIC_STONE,
    TILE_ICE_LIGHT_BREAKSTONE,
    TILE_FIRE_GROUND,
    TILE_FIRE_DARK_STONE,
    TILE_FIRE_LIGHT_BREAKSTONE,
    TILE_OVERWORLD_GRASS_GROUND,
    TILE_OVERWORLD_DIRT_GROUND,
    TILE_GHOST_STONE,
    TILE_COUNT
} Tile;

typedef enum {
    MAP_CAVE,
    MAP_ICE,
    MAP_FIRE,
    MAP_OVERWORLD,
    MAP_COUNT
} MapType;

typedef struct {
    int x;
    int y;
} MapCoords;

extern MapType g_current_map_type;
extern Tile g_maps[MAP_WIDTH][MAP_HEIGHT][MAP_COUNT];
extern bool g_is_ghost_stones_active;


Tile map_get(int map_x, int map_y);
void map_set(Tile tile, int map_x, int map_y);

void maps_load_from_folder();
void map_load_from_png(MapType id, char* file_path);
void map_switch(Player* p, MapType type);
void map_load_from_file(char* file_path);
void map_break_block(int map_x, int map_y, Tile end_tile);
void map_explode_block(int map_x, int map_y);
void map_dump();
void map_update(float delta_time);

#endif // _MAP_H_
