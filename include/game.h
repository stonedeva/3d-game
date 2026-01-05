#ifndef _GAME_H_
#define _GAME_H_

typedef enum {
    STATE_MENU = 0,
    STATE_INGAME,
    STATE_PAUSE,
    STATE_VICTORY,
    STATE_GAMEOVER,
} GameState;

extern GameState game_state;
extern int game_timer;

void game_reset(void);

#ifndef LARGE_SCREEN
    #define SCREEN_WIDTH 600
    #define SCREEN_HEIGHT 400
#else
    #define SCREEN_WIDTH 700
    #define SCREEN_HEIGHT 500
#endif // LARGE_SCREEN
#define TARGET_FPS 60

#endif // _GAME_H_
