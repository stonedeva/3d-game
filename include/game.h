#ifndef _GAME_H_
#define _GAME_H_

typedef enum {
    STATE_MENU = 0,
    STATE_INGAME,
    STATE_PAUSE
} GameState;

extern GameState game_state;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 450
#define TARGET_FPS 60

#endif // _GAME_H_
