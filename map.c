#include "./map.h"
#include "./sound.h"
#include <time.h>
#include <stdio.h>
#include <string.h>


Tile map[MAP_WIDTH][MAP_HEIGHT] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,2,2,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,2,0,0,0,0,0,2,1,0,0,0,0,1,1,1,0,0,0,1},
    {1,0,0,0,0,2,0,0,0,0,0,2,0,1,0,1,1,0,0,1,1,0,0,1},
    {1,0,0,0,2,0,0,0,0,0,0,3,0,0,1,0,0,0,0,0,1,0,0,1},
    {1,0,0,0,2,0,0,0,0,0,0,2,0,0,0,0,1,1,0,0,1,0,0,1},
    {1,0,0,0,0,2,2,0,2,2,2,0,1,0,0,1,0,1,0,0,1,0,0,1},
    {1,0,0,0,0,0,1,3,1,0,0,0,0,1,0,1,0,0,0,0,1,0,0,1},
    {1,0,0,0,0,0,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,0,0,1},
    {1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
    {1,0,0,0,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

unsigned char new_map[MAP_WIDTH][MAP_HEIGHT];


static int _map_lighten_color(int col, int degree)
{
    int r = ((col >> 16) & 0xFF) + ((255 - r) >> degree);
    int g = ((col >> 8) & 0xFF) + ((255 - g) >> degree);
    int b = (col & 0xFF) + ((255 - b) >> degree);

    int new_col = (r << 16) | (g << 8) | b;
    return new_col;
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
    int radius = 3;
    for (int y = map_y-radius; y < map_y+radius; y++) {
	for (int x = map_x-radius; x < map_x+radius; x++) {
	    map[x][y] = 0;
	}
    }
}

void map_dump()
{
    for (int y = 0; y < MAP_HEIGHT; y++) {
	printf("{");
	for (int x = 0; x < MAP_WIDTH; x++) {
	    printf("%d, ", map[x][y]);
	}
	printf("}\n");
    }
}

// Cellular Automaton
int map_get_neighbours(int x, int y)
{
    int n = 0;
    for (int dy = -1; dy <= 1; dy++) {
	for (int dx = -1; dx <= 1; dx++) {
	    if (dx == 0 && dy == 0) {
		continue;
	    }
	    int nx = dx + x;
	    int ny = dy + y;
	    if (nx < 0 || ny < 0 || nx >= MAP_WIDTH || ny >= MAP_HEIGHT) {
		n++;
	    } else if (map[nx][ny] == 1) {
		n++;
	    }
	}
    }
    return n;
}

void map_generate()
{
    for (int x = 0; x < MAP_WIDTH; x++) {
	for (int y = 0; y < MAP_HEIGHT; y++) {
	    if (map_get_neighbours(x, y) > 5) {
		new_map[x][y] = 1;
	    } else {
		new_map[x][y] = 0;
	    }
	}
    }
    memcpy(map, new_map, MAP_WIDTH*MAP_HEIGHT);
    map_dump();
}
