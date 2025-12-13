#ifndef _PLAYER_H_
#define _PLAYER_H_

#define PLAYER_SPEED 0.03
#define PLAYER_ROT_SPEED 0.03

typedef struct {
    double px;
    double py;
    double pa;
    double dir_x;
    double dir_y;
    double plane_x;
    double plane_y;
} Player;

/* TODO: Temporary solution to avoid circular includes */
#include "./screen.h"

Player player_init();
void player_forward(Player* p);
void player_backward(Player* p);
void player_rotate(Player* p, double units);
void player_handle_input(Screen* screen, Player* p);

#endif // _PLAYER_H_
