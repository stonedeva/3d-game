#include "./player.h"
#include "./map.h"
#include "./game.h"
#include "./sprite.h"
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
    if (map[(int)(p->pos.x + p->dir.x * PLAYER_SPEED)][(int)p->pos.y] == 0) 
	p->pos.x += p->dir.x * PLAYER_SPEED;
    if (map[(int)p->pos.x][(int)(p->pos.y + p->dir.y * PLAYER_SPEED)] == 0) 
	p->pos.y += p->dir.y * PLAYER_SPEED;
}

void player_backward(Player* p)
{
    if (map[(int)(p->pos.x - p->dir.x * PLAYER_SPEED)][(int)p->pos.y] == 0) 
	p->pos.x -= p->dir.x * PLAYER_SPEED;
    if (map[(int)p->pos.x][(int)(p->pos.y - p->dir.y * PLAYER_SPEED)] == 0) 
	p->pos.y -= p->dir.y * PLAYER_SPEED;
}

void player_rotate(Player* p, int dir)
{
    double old_dir_x = p->dir.x;
    p->dir = (Vec2) {
	p->dir.x * cos(dir*PLAYER_ROT_SPEED) - p->dir.y * sin(dir*PLAYER_ROT_SPEED),
	old_dir_x * sin(dir*PLAYER_ROT_SPEED) + p->dir.y * cos(dir*PLAYER_ROT_SPEED)
    };
    double old_plane_x = p->plane.x;

    p->plane = (Vec2) {
	p->plane.x * cos(dir*PLAYER_ROT_SPEED) - p->plane.y * sin(dir*PLAYER_ROT_SPEED),
	old_plane_x * sin(dir*PLAYER_ROT_SPEED) + p->plane.y * cos(dir*PLAYER_ROT_SPEED)
    };
}

void player_interact_block(Player* p, int map_x, int map_y)
{
    Tile tile = map[map_x][map_y];
    switch (tile) {
    case TILE_LIGHT_BREAKSTONE0:
    case TILE_LIGHT_BREAKSTONE1:
    case TILE_LIGHT_BREAKSTONE2:
    case TILE_LIGHT_BREAKSTONE3:
    case TILE_LIGHT_BREAKSTONE4:
	map_break_block(map_x, map_y, TILE_LIGHT_BREAKSTONE4);
	break;
    case TILE_TNT:
	map_explode_block(map_x, map_y);
	break;
    }
}

void player_update(Player* p)
{
    player_handle_input(p);
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
	player_rotate(p, PLAYER_ROT_RIGHT);
    }
    if (state[SDL_SCANCODE_LEFT]) {
	player_rotate(p, PLAYER_ROT_LEFT);
    }

    uint8_t current_space = state[SDL_SCANCODE_SPACE];
    static uint8_t prev_space = 0;
    if (prev_space && !current_space) {
	int map_x = (int)(p->pos.x + p->dir.x);
	int map_y = (int)(p->pos.y + p->dir.y);
	float dist = sqrtf(pow(p->pos.x - map_x, 2) + pow(p->pos.y - map_y, 2));

	if (dist <= PLAYER_ATTACK_RANGE) {
	   player_interact_block(p, map_x, map_y);
	}
    }
    prev_space = current_space;
}
