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


MapType current_map_type = MAP_CAVE;
Tile map[MAP_WIDTH][MAP_HEIGHT] = {0};

MapCoords ghost_stones[GHOST_STONE_CAP] = {0};
int ghost_stone_count = 0;
bool is_ghost_stones_active = 0;

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

void map_switch(Player* p, MapType type)
{
    current_map_type = type;

    p->pos.y += 0.5f;

    switch (type) {
    case MAP_CAVE:
	map_load_from_png("./res/maps/cave.png");
	break;
    case MAP_ICE:
	map_load_from_png("./res/maps/ice.png");
	break;
    case MAP_FIRE:
	map_load_from_png("./res/maps/fire.png");
	break;
    case MAP_OVERWORLD:
	memset(map, 0, sizeof(Tile)*MAP_WIDTH*MAP_HEIGHT);
	break;
    }
    sound_play(SOUND_LEVEL_ENTRANCE);
    SDL_Delay(250);
}

float plate_timer = 0.0f;

void map_update(float delta_time)
{
    plate_timer += delta_time;
    if (plate_timer >= 2.0f) {
	plate_timer = 0.0f;
	is_ghost_stones_active = !is_ghost_stones_active;
    }
}

void map_load_from_file(char* file_path)
{
    FILE* fp = fopen(file_path, "rb");
    if (!fp) {
	fprintf(stderr, "ERROR: fopen(): Failed to open map file: %s\n",
		strerror(errno));
	exit(1);
    }

    size_t n = fread(map, 1, MAP_WIDTH*MAP_HEIGHT, fp);
    if (n != MAP_WIDTH*MAP_HEIGHT) {
	fprintf(stderr, "ERROR: fread(): Expected %d bytes read %zu bytes: %s\n",
		MAP_WIDTH*MAP_HEIGHT, n, strerror(errno));
	exit(1);
    }

    fclose(fp);
}

void map_load_from_png(char* file_path)
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
		    map[x][y] = i;
		    if (i == TILE_GHOST_STONE && ghost_stone_count < GHOST_STONE_CAP) {
			ghost_stones[ghost_stone_count++] = (MapCoords){x,y};
		    }
		    break;
		} else {
		    // Important to avoid remaining tiles 
		    // from other levels being rendered
		    map[x][y] = 0;
		}
	    }
        }
    }
    SDL_FreeSurface(surface);
}

void map_break_block(int map_x, int map_y, Tile last_tile)
{
    Tile tile = map[map_x][map_y];
    if (tile != last_tile) {
	map[map_x][map_y] = tile + 1;
    } else {
	map[map_x][map_y] = TILE_EMPTY;
	sound_play(SOUND_WALL_DESTROY);
    }
}

void map_explode_block(int map_x, int map_y)
{
    assert(map[map_x][map_y] == TILE_FIRE_LIGHT_BREAKSTONE && 
	   "map_explode_block(): Expected TILE_FIRE_LIGHT_BREAKSTONE!\n");

    map[map_x][map_y] = TILE_EMPTY;
    sound_play(SOUND_EXPLOSION);

    map[map_x][map_y+1] = TILE_EMPTY;
    map[map_x][map_y-1] = TILE_EMPTY;
    map[map_x+1][map_y] = TILE_EMPTY;
    map[map_x-1][map_y] = TILE_EMPTY;
}

void map_dump()
{
    for (int x = 0; x < MAP_WIDTH; x++) {
	printf("{");
	for (int y = 0; y < MAP_HEIGHT; y++) {
	    printf("%d, ", map[x][y]);
	}
	printf("}\n");
    }
}
