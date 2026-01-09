#include "./map.h"
#include "./player.h"
#include "./sound.h"
#include "./bitmap.h"
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


MapType g_current_map_type = MAP_CAVE;
Tile g_maps[MAP_WIDTH][MAP_HEIGHT][MAP_COUNT] = {0};
bool g_is_ghost_stones_active = 0;

MapCoords ghost_stones[GHOST_STONE_CAP] = {0};
int ghost_stone_count = 0;

int map_colors[TILE_COUNT] = {
    [TILE_DARK_STONE] = 0xff252323,
    [TILE_LIGHT_STONE] = 0xff525050,
    [TILE_LIGHT_BREAKSTONE0] = 0xff727070,
    [TILE_DOOR] = 0xff63462a,
    [TILE_ICE_DARK_STONE] = 0xff59507d,
    [TILE_ICE_LIGHT_STONE] = 0xff8983a0,
    [TILE_MAGIC_STONE] = 0xff303030,
    [TILE_ICE_LIGHT_BREAKSTONE] = 0xff3e385b,
    [TILE_FIRE_DARK_STONE] = 0xff742134,
    [TILE_FIRE_LIGHT_BREAKSTONE] = 0xffa86575,
    [TILE_GHOST_STONE] = 0xffffdc00
};

Tile map_get(int map_x, int map_y)
{
    assert(g_current_map_type <= MAP_COUNT);
    return g_maps[map_x][map_y][g_current_map_type];
}

void map_set(Tile tile, int map_x, int map_y)
{
    assert(g_current_map_type <= MAP_COUNT);
    g_maps[map_x][map_y][g_current_map_type] = tile;
}

void maps_load_from_folder()
{
    map_load_from_png(MAP_CAVE, "./res/maps/cave.png");
    map_load_from_png(MAP_ICE, "./res/maps/ice.png");
    map_load_from_png(MAP_FIRE, "./res/maps/fire.png");
}

void map_switch(Player* p, MapType type)
{
    g_current_map_type = type;
    p->pos.y += 0.5f;
    sound_play(SOUND_LEVEL_ENTRANCE);
    SDL_Delay(250);
}

float plate_timer = 0.0f;

void map_update(float delta_time)
{
    plate_timer += delta_time;
    if (plate_timer >= 2.0f) {
	plate_timer = 0.0f;
	g_is_ghost_stones_active = !g_is_ghost_stones_active;
    }
}

void map_load_from_png(MapType id, char* file_path)
{
    SDL_Surface* surface = IMG_Load(file_path);
    if (!surface) {
        fprintf(stderr, "map_load_from_png(): Could not load map\n");
        exit(1);
    }
    
    uint8_t* p = (uint8_t*)surface->pixels;
    if (surface->format->BitsPerPixel != 24) {
        fprintf(stderr, "BitsPerPixel != 24: Could not read image file\n");
        SDL_FreeSurface(surface);
        exit(1);
    }

    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            uint8_t* px = p + x * surface->pitch + y * 3;
	    uint8_t r = px[0];
	    uint8_t g = px[1];
	    uint8_t b = px[2];
	    int color = (0xFF << 24) | (r << 16) | (g << 8) | b;

	    for (int i = 0; i < TILE_COUNT; i++) {
		if (map_colors[i] == color) {
		    g_maps[x][y][id] = i;
		    if (i == TILE_GHOST_STONE && ghost_stone_count < GHOST_STONE_CAP) {
			ghost_stones[ghost_stone_count++] = (MapCoords){x,y};
		    }
		    break;
		} else {
		    // Wichtig, damit nicht Blöcke von vorherigen Level gerendert werden
		    g_maps[x][y][id] = 0;
		}
	    }
        }
    }
    SDL_FreeSurface(surface);
}

void map_break_block(int map_x, int map_y, Tile last_tile)
{
    Tile tile = map_get(map_x, map_y);
    if (tile != last_tile) {
	map_set(tile+1, map_x, map_y);
    } else {
	map_set(TILE_EMPTY, map_x, map_y);
	sound_play(SOUND_WALL_DESTROY);
    }
}

void map_explode_block(int map_x, int map_y)
{
    assert(map_get(map_x, map_y) == TILE_FIRE_LIGHT_BREAKSTONE && 
	   "map_explode_block(): Expected TILE_FIRE_LIGHT_BREAKSTONE!\n");

    map_set(TILE_EMPTY, map_x, map_y);
    sound_play(SOUND_EXPLOSION);

    map_set(TILE_EMPTY, map_x, map_y+1);
    map_set(TILE_EMPTY, map_x, map_y-1);
    map_set(TILE_EMPTY, map_x+1, map_y);
    map_set(TILE_EMPTY, map_x-1, map_y);
}

void map_dump()
{
    for (int x = 0; x < MAP_WIDTH; x++) {
	printf("{");
	for (int y = 0; y < MAP_HEIGHT; y++) {
	    printf("%d, ", map_get(x, y));
	}
	printf("}\n");
    }
}
