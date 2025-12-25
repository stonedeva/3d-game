#include "./screen.h"
#include "./map.h"
#include "./player.h"
#include "./game.h"
#include "./sprite.h"
#include "./vec2.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

SpriteManager sprite_mgr = {0};


Screen screen_init(int* pixels)
{
    Screen screen = {0};
    screen.pixels = pixels;

    screen_load_texture(&screen, "./res/wall0.png");
    screen_load_texture(&screen, "./res/wall1.png");
    screen_load_texture(&screen, "./res/wall1_breakable0.png");
    screen_load_texture(&screen, "./res/wall1_breakable1.png");
    screen_load_texture(&screen, "./res/wall1_breakable2.png");
    screen_load_texture(&screen, "./res/wall1_breakable3.png");
    screen_load_texture(&screen, "./res/wall1_breakable4.png");
    screen_load_texture(&screen, "./res/gate.png");
    screen_load_texture(&screen, "./res/ground.png");

    sprite_load(&sprite_mgr, "./res/barrel.png", 22, 15);

    return screen;
}

void screen_load_texture(Screen* screen, char* file_path)
{
    SDL_Surface* surface = IMG_Load(file_path);
    if (!surface) {
	fprintf(stderr, "screen_load_texture(): Could not read image file\n");
	exit(1);
    }
    uint8_t* p = (uint8_t*)surface->pixels;

    /* 8 bit for r,g,b (0-255 each) */
    if (surface->format->BitsPerPixel != 24) {
	fprintf(stderr, "BitsPerPixel != 24: Could not read image file\n");
	SDL_FreeSurface(surface);
	exit(1);
    }

    GameImage img = {0};
    for (int y = 0; y < IMG_HEIGHT; y++) {
	for (int x = 0; x < IMG_WIDTH; x++) {
	    uint8_t* px = p + y * surface->pitch + x * surface->format->BytesPerPixel;
	    uint8_t r, g, b;
	    SDL_GetRGB(*(int*)px, surface->format, &r, &g, &b);
	    img.pixels[y * IMG_WIDTH + x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
	}
    }
    SDL_FreeSurface(surface);

    if (screen->texture_count >= TEXTURE_CAP) {
	fprintf(stderr, "TEXTURE_CAP has been reached!\n");
	exit(1);
    }
    screen->textures[screen->texture_count++] = img;
}


void screen_render_floor(Screen* screen, Vec2* dir, Vec2* plane, Vec2* pos)
{
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

	    int tx = (int)(IMG_WIDTH * (floor_x - cell_x)) & 15;
	    int ty = (int)(IMG_HEIGHT * (floor_y - cell_y)) & 15;

	    floor_x += step_x;
	    floor_y += step_y;

	    // Floor
	    int color = screen->textures[8].pixels[IMG_WIDTH * ty + tx];
	    color = (color >> 1) & 8355711; // More darker
	    screen->pixels[y * SCREEN_WIDTH + x] = color;
	
	    // Celling
	    screen->pixels[(SCREEN_HEIGHT - y - 1) * SCREEN_WIDTH + x] = color;
	}
    }
}

void screen_perform_dda(Ray* ray, int* map_x, int* map_y, Vec2* pos)
{
    int step_x, step_y;
    int hit = 0;
    bool is_invisible = 0;

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

	Tile tile = map[*map_x][*map_y];
	if (tile > 0) {
	    hit = 1;
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

	int tex_num = map[map_x][map_y] - 1;
	int tex_x = (int)(wall_x * (double)IMG_WIDTH);
	if (ray.side == 0 && ray_dir.x > 0) tex_x = IMG_WIDTH - tex_x - 1;
	if (ray.side == 1 && ray_dir.y < 0) tex_x = IMG_HEIGHT - tex_x - 1;

	int line_height = (int) (SCREEN_HEIGHT / ray.perp_wall_dist);
	int draw_start = -line_height / 2 + SCREEN_HEIGHT / 2;
	if (draw_start < 0) {
	    draw_start = 0;
	}
	int draw_end = line_height / 2 + SCREEN_HEIGHT / 2;
	if (draw_end >= SCREEN_HEIGHT) {
	    draw_end = SCREEN_HEIGHT - 1;
	}

	double step = 1.0 * IMG_HEIGHT / line_height;
	double tex_pos = (draw_start - SCREEN_HEIGHT / 2 + line_height / 2) * step;

	for (int y = draw_start; y < draw_end; y++) {
	    int tex_y = (int)tex_pos & (IMG_HEIGHT - 1);
	    tex_pos += step;
	    int color = screen->textures[tex_num].pixels[IMG_HEIGHT * tex_y + tex_x];
	    if (ray.side == 1) {
		color = (color >> 1) & 8355711;
	    }
	    if (color != 0xffff00ff) {
		screen->pixels[y * SCREEN_WIDTH + x] = color;
	    }
	}
    }
}

void screen_render(Screen* screen, Vec2* dir, Vec2* plane, Vec2* pos)
{
    screen_render_floor(screen, dir, plane, pos);
    screen_render_walls(screen, dir, plane, pos);
}
