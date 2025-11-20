#include "./screen.h"
#include "./game.h"
#include <stddef.h>
#include <stdint.h>
#include <math.h>

#define MAP_WIDTH 16
#define MAP_HEIGHT 16

static int bitmap[MAP_WIDTH][MAP_HEIGHT] = {
    // 0 = empty, 1 = wall
    {1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

void screen_render_map(int* pixels, double px, double py, double pa)
{
    for (size_t x = 0; x < SCREEN_WIDTH; x++) {
        double cameraX = 2.0 * x / SCREEN_WIDTH - 1.0;        // -1 to +1
        double ray_dir_x = cos(pa) + cameraX * -sin(pa);
        double ray_dir_y = sin(pa) + cameraX *  cos(pa);

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

            if (bitmap[map_x][map_y] > 0) hit = 1;
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
            pixels[y * SCREEN_WIDTH + x] = color;
	}
    }
}

void screen_render(int* pixels)
{
    /*
    for (size_t y = 0; y < SCREEN_HEIGHT; y++) {
	double yd = (y - SCREEN_HEIGHT / 2.0) / SCREEN_HEIGHT;
	if (yd == 0) {
	    continue;
	}

	double z = 6 / yd;

	for (size_t x = 0; x < SCREEN_WIDTH; x++) {
	    double xd = (z - SCREEN_WIDTH / 2.0) / SCREEN_HEIGHT;
	    xd *= z;
	    int xx = (int) (xd) & 15;
	    int zz = (int) (z) & 15;
	    
	    pixels[x + y * SCREEN_WIDTH] = (xx * 16) | (zz * 16) << 8;
	}
    }
    */

    screen_render_map(pixels, 0, 2.5, 0.4);
}
