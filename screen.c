#include "./screen.h"
#include "./map.h"
#include "./player.h"
#include "./game.h"
#include "./item.h"
#include "./vec2.h"
#include "./ladder.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include <time.h>


double zbuffer[SCREEN_WIDTH] = {0};
void screen_generate_overworld_floor();

Screen screen_init(int* pixels)
{
    Screen screen = {0};
    screen.pixels = pixels;

    screen.bitmap = bitmap_load("./res/bitmap.png");
    screen_generate_overworld_floor();

    return screen;
}

int overworld_floor_map[MAP_WIDTH][MAP_HEIGHT] = {0};

void screen_generate_overworld_floor()
{
    srand(time(0));
    for (int x = 0; x < MAP_WIDTH; x++) {
	for (int y = 0; y < MAP_HEIGHT; y++) {
	    double r = (double)rand() / (double)RAND_MAX; // 0 - 1, 0% - 100%
	    if (r < 0.2) {
		overworld_floor_map[x][y] = 1;
	    } else {
		overworld_floor_map[x][y] = 0;
	    }
	}
    }
}

void screen_render_floor(Screen* screen, Vec2* dir, Vec2* plane, Vec2* pos)
{
#ifndef LARGE_SCREEN
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
	Vec2 ray_dir0 = {
	    dir->x - plane->x,
	    dir->y - plane->y
	};
	Vec2 ray_dir1 = {
	    dir->x + plane->x,
	    dir->y + plane->y
	};

	int p = y - SCREEN_HEIGHT / 2;
	float z = (float)SCREEN_HEIGHT / 2;
	float row_dist = z / p;

	float step_x = row_dist * (ray_dir1.x - ray_dir0.x) / SCREEN_WIDTH;
	float step_y = row_dist * (ray_dir1.y - ray_dir0.y) / SCREEN_WIDTH;

	float floor_x = pos->x + row_dist * ray_dir0.x;
	float floor_y = pos->y + row_dist * ray_dir0.y;

	for (int x = 0; x < SCREEN_WIDTH; x++) {
	    int cell_x = (int)floor_x;
	    int cell_y = (int)floor_y;

	    int tx = (int)(TEX_WIDTH * (floor_x - cell_x)) & 15;
	    int ty = (int)(TEX_HEIGHT * (floor_y - cell_y)) & 15;

	    floor_x += step_x;
	    floor_y += step_y;

	    // Floor
	    int bm_floor_index = 0;
	    int bm_celling_index = 0;
	    switch (current_map_type) {
	    case MAP_CAVE:
		bm_floor_index = 0;
		break;
	    case MAP_ICE:
		bm_floor_index = TILE_ICE_GROUND;
		break;
	    case MAP_FIRE:
		bm_floor_index = TILE_FIRE_GROUND;
		break;
	    case MAP_OVERWORLD:
		if (overworld_floor_map[cell_x % MAP_WIDTH][cell_y % MAP_HEIGHT] == 0) {
		    bm_floor_index = TILE_OVERWORLD_GRASS_GROUND;
		} else {
		    bm_floor_index = TILE_OVERWORLD_DIRT_GROUND;
		}
		break;
	    }
	    bm_celling_index = bm_floor_index;
	    if (map[cell_x % MAP_WIDTH][cell_y % MAP_HEIGHT] == TILE_SECRET_PLATE) {
		bm_floor_index = TILE_SECRET_PLATE;
	    }

	    // Floor
	    int color = screen->bitmap.pixels[TEX_WIDTH * ty + tx][bm_floor_index];
	    color = (color >> 1) & 8355711; // More darker
	    screen->pixels[y * SCREEN_WIDTH + x] = color;

	    // Celling
	    if (current_map_type == MAP_OVERWORLD) {
		screen->pixels[(SCREEN_HEIGHT - y - 1) * SCREEN_WIDTH + x] = 0;
	    } else {
		screen->pixels[(SCREEN_HEIGHT - y - 1) * SCREEN_WIDTH + x] = color;
	    }
	}
    }
#else
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
	for (int x = 0; x < SCREEN_WIDTH; x++) {
	    if (y > SCREEN_HEIGHT / 2) {
		screen->pixels[y * SCREEN_WIDTH + x] = 0x555555;
	    } else {
		screen->pixels[y * SCREEN_WIDTH + x] = 0x222222;
	    }
	}
    }
#endif // LARGE_SCREEN
}

void screen_perform_dda(Ray* ray, int* map_x, int* map_y, Vec2* pos)
{
    int step_x, step_y;
    int hit = 0;

    if (ray->dir.x < 0) {
	step_x = -1;
	ray->side_dist.x = (pos->x - *map_x) * ray->delta_dist.x;
    } else {
	step_x = 1;
	ray->side_dist.x = (*map_x + 1.0 - pos->x) * ray->delta_dist.x;
    }
    if (ray->dir.y < 0) {
	step_y = -1;
	ray->side_dist.y = (pos->y - *map_y) * ray->delta_dist.y;
    } else {
	step_y = 1;
	ray->side_dist.y = (*map_y + 1.0 - pos->y) * ray->delta_dist.y;
    }

    while (hit == 0) {
	if (ray->side_dist.x < ray->side_dist.y) {
	    ray->side_dist.x += ray->delta_dist.x;
	    *map_x += step_x;
	    ray->side = 0;
	} else {
	    ray->side_dist.y += ray->delta_dist.y;
	    *map_y += step_y;
	    ray->side = 1;
	}

	if (*map_x < 0 || *map_x >= MAP_WIDTH ||
	    *map_y < 0 || *map_y >= MAP_HEIGHT) {
	    hit = 1;
	}

	if (map[*map_x][*map_y] > 0 &&
	    (map[*map_x][*map_y] != TILE_SECRET_PLATE || is_plates_active)) {
	    hit = 1;
	    break;
	}
    }
}

void screen_calculate_perp_wall_dist(Ray* ray, Vec2* pos, double* wall_x)
{
    if (ray->side == 0) {
	ray->perp_wall_dist = (ray->side_dist.x - ray->delta_dist.x);
    } else {
	ray->perp_wall_dist = (ray->side_dist.y - ray->delta_dist.y);
    }
 
    if (ray->side == 0) {
	*wall_x = pos->y + ray->perp_wall_dist * ray->dir.y;
    } else {
        *wall_x = pos->x + ray->perp_wall_dist * ray->dir.x;
    }
    *wall_x -= floor((*wall_x));
}

void screen_render_walls(Screen* screen, Vec2* dir, Vec2* plane, Vec2* pos)
{
    for (int x = 0; x < SCREEN_WIDTH; x++) {
	double camera_x = 2 * x / (double)SCREEN_WIDTH - 1;
	int map_x = (int)pos->x;
	int map_y = (int)pos->y;

	Vec2 ray_dir = {dir->x + plane->x * camera_x,
			dir->y + plane->y * camera_x};
	Vec2 side_dist = {0};
	Vec2 delta_dist = {(ray_dir.x == 0) ? 1e30 : fabs(1 / ray_dir.x),
			   (ray_dir.y == 0) ? 1e30 : fabs(1 / ray_dir.y)};
	Ray ray = {
	    ray_dir, side_dist, delta_dist
	};

	screen_perform_dda(&ray, &map_x, &map_y, pos);

	double wall_x = 0;
	screen_calculate_perp_wall_dist(&ray, pos, &wall_x);

	int tex_num = map[map_x][map_y];

	int tex_x = (int)(wall_x * (double)TEX_WIDTH);
	if (ray.side == 0 && ray_dir.x > 0) tex_x = TEX_WIDTH - tex_x - 1;
	if (ray.side == 1 && ray_dir.y < 0) tex_x = TEX_HEIGHT - tex_x - 1;

	int line_height = (int) (SCREEN_HEIGHT / ray.perp_wall_dist);
	int draw_start = -line_height / 2 + SCREEN_HEIGHT / 2;
	if (draw_start < 0) {
	    draw_start = 0;
	}
	int draw_end = line_height / 2 + SCREEN_HEIGHT / 2;
	if (draw_end >= SCREEN_HEIGHT) {
	    draw_end = SCREEN_HEIGHT - 1;
	}

	double step = 1.0 * TEX_HEIGHT / line_height;
	double tex_pos = (draw_start - SCREEN_HEIGHT / 2 + line_height / 2) * step;
	
	for (int y = draw_start; y < draw_end; y++) {
	    int tex_y = (int)tex_pos & (TEX_HEIGHT - 1);
	    tex_pos += step;
	    int color = screen->bitmap.pixels[TEX_HEIGHT * tex_y + tex_x][tex_num];
	    if (ray.side == 1) {
		color = (color >> 1) & 8355711;
	    }
	    screen->pixels[y * SCREEN_WIDTH + x] = color;
	}
	zbuffer[x] = ray.perp_wall_dist;
    }
}

void screen_render(Screen* screen, Vec2* dir, Vec2* plane, Vec2* pos)
{
    screen_render_floor(screen, dir, plane, pos);
    if (current_map_type != MAP_OVERWORLD)
	screen_render_walls(screen, dir, plane, pos);
    items_render(screen, dir, plane, pos);
    ladders_render(screen, dir, plane, pos);
}
