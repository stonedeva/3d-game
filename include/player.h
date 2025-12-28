#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "./vec2.h"
#include "./sprite.h"
#include <stdbool.h>

#define PLAYER_SPEED 0.03
#define PLAYER_ROT_SPEED 0.03
#define PLAYER_ROT_RIGHT -1
#define PLAYER_ROT_LEFT 1
#define PLAYER_ATTACK_RANGE 8
#define PLAYER_COOLDOWN_START 25.0f
#define PLAYER_MAX_HEALTH 10

typedef struct {
    float cooldown;
    int health;
    bool has_key;
    Vec2 pos;
    Vec2 dir;
    Vec2 plane;
} Player;


Player player_init();
void player_forward(Player* p);
void player_backward(Player* p);
void player_rotate(Player* p, int dir);
void player_handle_input(Player* p);
void player_render(int* pixels, Player* p);
void player_pickup_item(Player* p, int item_index);
void player_update(Player* p);
bool player_check_collision(Player* p, Sprite* sprite, double radius);
void player_take_damage(Player* p, int damage);

#endif // _PLAYER_H_
