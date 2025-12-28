#ifndef _GAME_H_
#define _GAME_H_

typedef enum {
    STATE_MENU = 0,
    STATE_INGAME,
    STATE_PAUSE
} GameState;

extern GameState game_state;

#ifndef LARGE_SCREEN
    #define SCREEN_WIDTH 650
    #define SCREEN_HEIGHT 450
#else
    #define SCREEN_WIDTH 700
    #define SCREEN_HEIGHT 500
#endif // LARGE_SCREEN
#define TARGET_FPS 60

#endif // _GAME_H_
