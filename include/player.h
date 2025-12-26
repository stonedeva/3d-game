#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "./vec2.h"

#define PLAYER_SPEED 0.03
#define PLAYER_ROT_SPEED 0.03
#define PLAYER_ROT_RIGHT -1
#define PLAYER_ROT_LEFT 1
#define PLAYER_ATTACK_RANGE 8

typedef struct {
    Vec2 pos;
    double pa;
    Vec2 dir;
    Vec2 plane;
} Player;


Player player_init();
void player_forward(Player* p);
void player_backward(Player* p);
void player_rotate(Player* p, int dir);
void player_handle_input(Player* p);
void player_update(Player* p);

#endif // _PLAYER_H_
