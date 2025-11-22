#include "./screen.h"
#include "./player.h"
#include "./game.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>


Screen screen_init(int* pixels)
{
    return (Screen) {
	.pixels = pixels,
	.pos_x = 22,
	.pos_y = 12,
	.dir_x = -1,
	.dir_y = 0,
	.plane_x = 0,
	.plane_y = 0.66
    };
}
/*
void screen_render_map(Screen* screen, double px, double py, double pa)
{
    for (size_t x = 0; x < SCREEN_WIDTH; x++) {
        double camera_x = 2.0 * x / SCREEN_WIDTH - 1.0;        // -1 to +1
        double ray_dir_x = cos(pa) + camera_x * -sin(pa);
        double ray_dir_y = sin(pa) + camera_x *  cos(pa);

        int map_x = (int)px;
        int map_y = (int)py;

        double delta_dist_x = (ray_dir_x == 0) ? 1e30 : fabs(1 / ray_dir_x);
        double delta_dist_y = (ray_dir_y == 0) ? 1e30 : fabs(1 / ray_dir_y);

        int step_x, step_y;
        double side_dist_x, side_dist_y;

        if (ray_dir_x < 0) {
            step_x = -1;
            side_dist_x = (px - map_x) * delta_dist_x;
        } else {
            step_x = 1;
            side_dist_x = (map_x + 1.0 - px) * delta_dist_x;
        }

        if (ray_dir_y < 0) {
            step_y = -1;
            side_dist_y = (py - map_y) * delta_dist_y;
        } else {
            step_y = 1;
            side_dist_y = (map_y + 1.0 - py) * delta_dist_y;
        }

        int hit = 0;
        int side;
        while (!hit) {
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

        double perp_dist =
            side == 0 ? (side_dist_x - delta_dist_x)
                      : (side_dist_y - delta_dist_y);

        int line_height = (int)(SCREEN_HEIGHT / perp_dist);

        int draw_start = -line_height / 2 + SCREEN_HEIGHT / 2;
        if (draw_start < 0) {
	    draw_start = 0;
	}

        int draw_end = line_height / 2 + SCREEN_HEIGHT / 2;
        if (draw_end >= SCREEN_HEIGHT) {
	    draw_end = SCREEN_HEIGHT - 1;
	}

        uint32_t color = side ? 0xAA0000 : 0xFF0000;

        for (int y = draw_start; y < draw_end; y++) {
            screen->pixels[y * SCREEN_WIDTH + x] = color;
	}
    }
}
*/

void screen_render_map(Screen* screen, Player* player)
{
    double pa = 1.5;

    for (size_t x = 0; x < SCREEN_WIDTH; x++) {
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

	int color;
	switch (screen_map[map_x][map_y]) {
	case 1:
	    color = 0xFF0000;
	    break;
	case 2:
	    color = 0x00FF00;
	    break;
	case 3:
	    color = 0x0000FF;
	    break;
	case 4:
	    color = 0xFFFF00;
	    break;
	}

	if (side == 1) {
	    color = color / 2;
	}

	for (int y = draw_start; y <= draw_end; y++) {
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

