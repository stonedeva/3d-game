#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "./vec2.h"

#define PLAYER_SPEED 0.03
#define PLAYER_ROT_SPEED 0.03

typedef struct {
    Vec2 pos;
    double pa;
    Vec2 dir;
    Vec2 plane;
} Player;

/* TODO: Temporary solution to avoid circular includes */
#include "./screen.h"

Player player_init();
void player_forward(Player* p);
void player_backward(Player* p);
void player_rotate(Player* p, double units);
void player_handle_input(Player* p);

#endif // _PLAYER_H_
