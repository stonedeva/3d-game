#include "./player.h"
#include "./map.h"
#include "./game.h"
#include "./sound.h"
#include "./sprite.h"
#include "./item.h"
#include "./bitmap.h"
#include <SDL2/SDL.h>
#include <assert.h>

#define TESTING_MODE

Player player_init()
{
    Player p = {0};
    p.pos = (Vec2) {22,12};
    p.dir = (Vec2) {-1, 0};
    p.plane = (Vec2) {0, 0.66};
#ifdef TESTING_MODE
    p.keys = 200;
    p.has_axe = 1;
    p.has_torch = 1;
#endif // TESTING_MODE
    return p;
}

bool player_check_victory(Player* p)
{
    return current_map_type == MAP_OVERWORLD && p->pos.y >= 30;
}

void player_forward(Player* p)
{
    int px0 = (int)(p->pos.x + p->dir.x * PLAYER_SPEED);
    int py0 = (int)p->pos.y;
    if (map[px0][py0] == TILE_EMPTY || map[px0][py0] == TILE_MAGIC_STONE ||
	map[px0][py0] == TILE_GHOST_STONE) 
	p->pos.x += p->dir.x * PLAYER_SPEED;
    int px1 = (int)p->pos.x;
    int py1 = (int)(p->pos.y + p->dir.y * PLAYER_SPEED);
    if (map[px1][py1] == TILE_EMPTY || map[px1][py1] == TILE_MAGIC_STONE ||
	map[px1][py1] == TILE_GHOST_STONE) 
	p->pos.y += p->dir.y * PLAYER_SPEED;
}

void player_backward(Player* p)
{
    int px0 = (int)(p->pos.x - p->dir.x * PLAYER_SPEED);
    int py0 = (int)p->pos.y;
    if (map[px0][py0] == TILE_EMPTY || map[px0][py0] == TILE_MAGIC_STONE ||
	map[px0][py0] == TILE_GHOST_STONE)
	p->pos.x -= p->dir.x * PLAYER_SPEED;
    int px1 = (int)p->pos.x;
    int py1 = (int)(p->pos.y - p->dir.y * PLAYER_SPEED);
    if (map[px1][py1] == TILE_EMPTY || map[px1][py1] == TILE_MAGIC_STONE ||
	map[px1][py1] == TILE_GHOST_STONE)
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
    case TILE_ICE_LIGHT_BREAKSTONE:
	if (p->has_axe) {
	    map[map_x][map_y] = 0;
	    sound_play(SOUND_WALL_DESTROY);
	} else {
	    sound_play(SOUND_WRONG);
	}
	break;
    case TILE_DOOR:
	if (p->keys > 0) {
	    map[map_x][map_y] = 0;
	    p->keys--;
	    sound_play(SOUND_DOOR_OPEN);
	} else {
	    sound_play(SOUND_WRONG);
	}
	break;
    case TILE_FIRE_LIGHT_BREAKSTONE:
	map_explode_block(map_x, map_y);
	break;
    default:
	break;
    }
}

bool player_check_collision(Player* p, Sprite* sprite, double radius)
{
    double dx = p->pos.x - sprite->pos.x;
    double dy = p->pos.y - sprite->pos.y;
    double dist = sqrt(dx*dx + dy*dy);

    return dist < radius;
}

void player_game_over(Player* p)
{
    game_state = STATE_GAMEOVER;
    sound_play(SOUND_VICTORY);

    game_timer = -1; /* Sound mehrfach abspielen vermeiden */
}

void player_victory(Player* p)
{
    game_state = STATE_VICTORY;
    sound_play(SOUND_GAMEOVER);
}

void player_pickup_item(Player* p, int item_index)
{
    if (item_index >= item_count) {
	return;
    }
    if (items[item_index].map != current_map_type) {
	return;
    }

    switch (items[item_index].type) {
    case ITEM_KEY:
	p->keys++;
	sound_play(SOUND_PICKUP_KEY);
	items[item_index] = (Item) {.type = ITEM_EMPTY};
	break;
    case ITEM_TIMEKIT:
	game_timer += 60;
	sound_play(SOUND_TIMEKIT);
	items[item_index] = (Item) {.type = ITEM_EMPTY};
	break;
    case ITEM_AXE:
	p->has_axe = 1;
	items[item_index] = (Item) {.type = ITEM_EMPTY};
	break;
    case ITEM_TORCH:
	p->has_torch = 1;
	items[item_index] = (Item) {.type = ITEM_EMPTY};
	break;
    case ITEM_FAKEKIT:
	if (game_timer < 30)
	    break;
	game_timer -= (int)(game_timer / 4);
	items[item_index] = (Item) {.type = ITEM_EMPTY};
	sound_play(SOUND_WRONG);
	break;
    default:
	break;
    }
}

void player_update(Player* p)
{
    if (map[(int)p->pos.x][(int)p->pos.y] == TILE_GHOST_STONE && is_ghost_stones_active) {
	player_game_over(p);
    }
    player_handle_input(p);
}

void player_handle_input(Player* p)
{
    uint8_t* keystate = SDL_GetKeyboardState(0);

    if (keystate[SDL_SCANCODE_W]) {
	player_forward(p);
    }
    if (keystate[SDL_SCANCODE_S]) {
	player_backward(p);
    }
    if (keystate[SDL_SCANCODE_RIGHT]) {
	player_rotate(p, PLAYER_ROT_RIGHT);
    }
    if (keystate[SDL_SCANCODE_LEFT]) {
	player_rotate(p, PLAYER_ROT_LEFT);
    }

    uint8_t current_space = keystate[SDL_SCANCODE_SPACE];
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
