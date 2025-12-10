#include "./screen.h"
#include "./player.h"
#include "./game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>


Screen screen_init(int* pixels)
{
    Screen screen = {0};
    screen.pixels = pixels;

    screen.wall_tex = screen_read_image("./res/wall.png");
    screen.ground_tex = screen_read_image("./res/ground.png");

    return screen;
}

GameImage screen_read_image(char* file_path)
{
    SDL_Surface* surface = IMG_Load(file_path);
    if (!surface) {
	fprintf(stderr, "screen_read_image(): Could not read image file\n");
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

    //memcpy(img.pixels, surface->pixels, IMG_WIDTH * IMG_HEIGHT * sizeof(int));
    SDL_FreeSurface(surface);

    printf("%02x\n", img.pixels[0]);

    return img;
}

void screen_render_floor(Screen* screen, Player* player)
{
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
	float rdx0 = player->dir_x - player->plane_x;
	float rdx1 = player->dir_x + player->plane_x;
	float rdy0 = player->dir_y - player->plane_y;
	float rdy1 = player->dir_y + player->plane_y;

	int p = y - SCREEN_HEIGHT / 2;
	float z = (float)SCREEN_HEIGHT / 2;
	float row_dist = z / p;

	float step_x = row_dist * (rdx1 - rdx0) / SCREEN_WIDTH;
	float step_y = row_dist * (rdy1 - rdy0) / SCREEN_WIDTH;

	float floor_x = player->px + row_dist * rdx0;
	float floor_y = player->py + row_dist * rdy0;

	for (int x = 0; x < SCREEN_WIDTH; x++) {
	    int cell_x = (int)floor_x;
	    int cell_y = (int)floor_y;

	    int tx = (int)(IMG_WIDTH * (floor_x - cell_x)) & 15;
	    int ty = (int)(IMG_HEIGHT * (floor_y - cell_y)) & 15;

	    floor_x += step_x;
	    floor_y += step_y;

	    // Floor
	    int color = screen->ground_tex.pixels[IMG_WIDTH * ty + tx];
	    color = (color >> 1) & 8355711; // More darker
	    screen->pixels[y * SCREEN_WIDTH + x] = color;
	
	    // Celling
	    screen->pixels[(SCREEN_HEIGHT - y - 1) * SCREEN_WIDTH + x] = color;
	}
    }
}

// TODO: Function is too long. Seperate into multiple functions
void screen_render_map(Screen* screen, Player* player)
{
    screen_render_floor(screen, player);

    for (int x = 0; x < SCREEN_WIDTH; x++) {
	double camera_x = 2 * x / (double)SCREEN_WIDTH - 1;
	double ray_dir_x = player->dir_x + player->plane_x * camera_x;
	double ray_dir_y = player->dir_y + player->plane_y * camera_x;

	int map_x = (int)player->px;
	int map_y = (int)player->py;

	double side_dist_x, side_dist_y;
	double delta_dist_x = (ray_dir_x == 0) ? 1e30 : fabs(1 / ray_dir_x);
	double delta_dist_y = (ray_dir_y == 0) ? 1e30 : fabs(1 / ray_dir_y);
	double perp_wall_dist;

	int step_x, step_y;
	int hit = 0;
	int side;

	if (ray_dir_x < 0) {
	    step_x = -1;
	    side_dist_x = (player->px - map_x) * delta_dist_x;
	} else {
	    step_x = 1;
	    side_dist_x = (map_x + 1.0 - player->px) * delta_dist_x;
	}
	if (ray_dir_y < 0) {
	    step_y = -1;
	    side_dist_y = (player->py - map_y) * delta_dist_y;
	} else {
	    step_y = 1;
	    side_dist_y = (map_y + 1.0 - player->py) * delta_dist_y;
	}

	while (hit == 0) {
	    if (side_dist_x < side_dist_y) {
		side_dist_x += delta_dist_x;
		map_x += step_x;
		side = 0;
	    } else {
		side_dist_y += delta_dist_y;
		map_y += step_y;
		side = 1;
	    }

	    if (screen_map[map_x][map_y] > 0) hit = 1;
	}
	
	if (side == 0)
	    perp_wall_dist = (side_dist_x - delta_dist_x);
	else
	    perp_wall_dist = (side_dist_y - delta_dist_y);

	int line_height = (int) (SCREEN_HEIGHT / perp_wall_dist);

	int draw_start = -line_height / 2 + SCREEN_HEIGHT / 2;
	if (draw_start < 0) {
	    draw_start = 0;
	}
	int draw_end = line_height / 2 + SCREEN_HEIGHT / 2;
	if (draw_end >= SCREEN_HEIGHT) {
	    draw_end = SCREEN_HEIGHT - 1;
	}
	
	int tex_num = 0;
	double wall_x;
	if (side == 0) {
	    wall_x = player->py + perp_wall_dist * ray_dir_y;
	} else {
	    wall_x = player->px + perp_wall_dist * ray_dir_x;
	}
	wall_x -= floor((wall_x));

	int tex_x = (int)(wall_x * (double)IMG_WIDTH);
	if (side == 0 && ray_dir_x > 0) tex_x = IMG_WIDTH - tex_x - 1;
	if (side == 1 && ray_dir_y < 0) tex_x = IMG_HEIGHT - tex_x - 1;

	double step = 1.0 * IMG_HEIGHT / line_height;
	double tex_pos = (draw_start - SCREEN_HEIGHT / 2 + line_height / 2) * step;

	for (int y = draw_start; y < draw_end; y++) {
	    int tex_y = (int)tex_pos & (IMG_HEIGHT - 1);
	    tex_pos += step;
	    int color = screen->wall_tex.pixels[IMG_HEIGHT * tex_y + tex_x];
	    if (side == 1) color = (color >> 1) & 8355711;
	    
	    screen->pixels[y * SCREEN_WIDTH + x] = color;
	}
    }
}

void screen_clear(Screen* screen)
{
    for (size_t i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
	screen->pixels[i] = 0;
    }
}

