#ifndef _MENU_H_
#define _MENU_H_

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "./game.h"

typedef enum {
    OPT_START = 0,
    OPT_CONTROL,
    OPT_QUIT,
    OPT_COUNT
} Options;


void menu_init();
void menu_render(SDL_Renderer* renderer);
void menu_handle_input(SDL_Event* ev);

#endif // _MENU_H_
