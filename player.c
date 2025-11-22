#include "./player.h"
#include "./screen.h"
#include "./game.h"
#include <SDL2/SDL.h>

Player player_init()
{
    return (Player) {
	.px = 0,
	.py = 2.5,
    };
}

/*
void player_forward(Player* p)
{
    double nx = p->px + cos(p->pa) * PLAYER_SPEED;
    double ny = p->py + sin(p->pa) * PLAYER_SPEED;

    if (screen_map[(int)nx][(int)p->py] == 0) p->px = nx;
    if (screen_map[(int)p->px][(int)ny] == 0) p->py = ny;
}

void player_backward(Player* p)
{
    double nx = p->px - cos(p->pa) * PLAYER_SPEED;
    double ny = p->py - sin(p->pa) * PLAYER_SPEED;
    
    if (screen_map[(int)nx][(int)p->py] == 0) p->px = nx;
    if (screen_map[(int)p->px][(int)ny] == 0) p->py = ny;
}

void player_rotate(Player* p, double units)
{
    p->pa += units;

    if (p->pa < 0) p->pa += 2 * M_PI;
    if (p->pa >= 2 * M_PI) p->pa -= 2 * M_PI;
}*/

void player_handle_input(Screen* screen, Player* p)
{
    double dir_x = p->dir_x;
    double dir_y = p->dir_y;
    double plane_x = p->plane_x;
    double plane_y = p->plane_y;
    
    uint8_t* state = SDL_GetKeyboardState(0);

    if (state[SDL_SCANCODE_W]) {
	if (screen_map[(int)(p->px + dir_x * PLAYER_SPEED)][(int)p->py] == 0) p->px += dir_x * PLAYER_SPEED;
	if (screen_map[(int)p->px][(int)(p->py + dir_y * PLAYER_SPEED)] == 0) p->py += dir_y * PLAYER_SPEED;
    }
    if (state[SDL_SCANCODE_S]) {
	if (screen_map[(int)(p->px - dir_x * PLAYER_SPEED)][(int)p->py] == 0) p->px -= dir_x * PLAYER_SPEED;
	if (screen_map[(int)p->px][(int)(p->py - dir_y * PLAYER_SPEED)] == 0) p->py -= dir_y * PLAYER_SPEED;
    }
    if (state[SDL_SCANCODE_RIGHT]) {
	double old_dir_x = dir_x;
	dir_x = dir_x * cos(-PLAYER_ROT_SPEED) - dir_y * sin(-PLAYER_ROT_SPEED);
	dir_y = old_dir_x * sin(-PLAYER_ROT_SPEED) + dir_y * cos(-PLAYER_ROT_SPEED);
	double old_plane_x = plane_x;
	plane_x = plane_x * cos(-PLAYER_ROT_SPEED) - plane_y * sin(-PLAYER_ROT_SPEED);
	plane_y = old_plane_x * sin(-PLAYER_ROT_SPEED) + plane_y * cos(-PLAYER_ROT_SPEED);
    }
    if (state[SDL_SCANCODE_LEFT]) {
	double old_dir_x = dir_x;
	dir_x = dir_x * cos(PLAYER_ROT_SPEED) - dir_y * sin(PLAYER_ROT_SPEED);
	dir_y = old_dir_x * sin(PLAYER_ROT_SPEED) + dir_y * cos(PLAYER_ROT_SPEED);
	double old_plane_x = plane_x;
	plane_x = plane_x * cos(PLAYER_ROT_SPEED) - plane_y * sin(PLAYER_ROT_SPEED);
	plane_y = old_plane_x * sin(PLAYER_ROT_SPEED) + plane_y * cos(PLAYER_ROT_SPEED);
    }

    p->dir_x = dir_x;
    p->dir_y = dir_y;
    p->plane_x = plane_x;
    p->plane_y = plane_y;
}
