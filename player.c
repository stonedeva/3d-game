#include "./player.h"
#include "./screen.h"
#include "./game.h"
#include <stdbool.h>
#include <SDL2/SDL.h>

Player player_init()
{
    Player p = {0};
    p.pos = (Vec2) {22, 12};
    p.pa = 1.5;
    p.dir = (Vec2) {-1, 0};
    p.plane = (Vec2) {0, 0.66};
    return p;
}

void player_forward(Player* p)
{
    if (screen_map[(int)(p->pos.x + p->dir.x * PLAYER_SPEED)][(int)p->pos.y] == 0) 
	p->pos.x += p->dir.x * PLAYER_SPEED;
    if (screen_map[(int)p->pos.x][(int)(p->pos.y + p->dir.y * PLAYER_SPEED)] == 0) 
	p->pos.y += p->dir.y * PLAYER_SPEED;
}

void player_backward(Player* p)
{
    if (screen_map[(int)(p->pos.x - p->dir.x * PLAYER_SPEED)][(int)p->pos.y] == 0) 
	p->pos.x -= p->dir.x * PLAYER_SPEED;
    if (screen_map[(int)p->pos.x][(int)(p->pos.y - p->dir.y * PLAYER_SPEED)] == 0) 
	p->pos.y -= p->dir.y * PLAYER_SPEED;
}

void player_handle_input(Player* p)
{
    uint8_t* state = SDL_GetKeyboardState(0);

    if (state[SDL_SCANCODE_W]) {
	player_forward(p);
    }
    if (state[SDL_SCANCODE_S]) {
	player_backward(p);
    }
    if (state[SDL_SCANCODE_RIGHT]) {
	double old_dir_x = p->dir.x;
	p->dir.x = p->dir.x * cos(-PLAYER_ROT_SPEED) - p->dir.y * sin(-PLAYER_ROT_SPEED);
	p->dir.y = old_dir_x * sin(-PLAYER_ROT_SPEED) + p->dir.y * cos(-PLAYER_ROT_SPEED);
	double old_plane_x = p->plane.x;
	p->plane.x = p->plane.x * cos(-PLAYER_ROT_SPEED) - p->plane.y * sin(-PLAYER_ROT_SPEED);
	p->plane.y = old_plane_x * sin(-PLAYER_ROT_SPEED) + p->plane.y * cos(-PLAYER_ROT_SPEED);
    }
    if (state[SDL_SCANCODE_LEFT]) {
	double old_dir_x = p->dir.x;
	p->dir.x = p->dir.x * cos(PLAYER_ROT_SPEED) - p->dir.y * sin(PLAYER_ROT_SPEED);
	p->dir.y = old_dir_x * sin(PLAYER_ROT_SPEED) + p->dir.y * cos(PLAYER_ROT_SPEED);
	double old_plane_x = p->plane.x;
	p->plane.x = p->plane.x * cos(PLAYER_ROT_SPEED) - p->plane.y * sin(PLAYER_ROT_SPEED);
	p->plane.y = old_plane_x * sin(PLAYER_ROT_SPEED) + p->plane.y * cos(PLAYER_ROT_SPEED);
    }
}
