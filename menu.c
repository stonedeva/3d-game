#include "./menu.h"
#include "./game.h"
#include "./sound.h"
#include <SDL2/SDL_ttf.h>


SDL_Color white = {255, 255, 255, 255};
SDL_Color gray = {200, 200, 200, 255};
SDL_Color yellow = {204, 204, 0, 255};
int selected = 0;

char* options[OPT_COUNT] = {
    "Start",
    "Control",
    "Quit"
};

void menu_render_text(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, 
		      char* text, int x, int y, int w, int h)
{
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) {
	fprintf(stderr, "ERROR: TTF_RenderText_Solid(): %s\n", TTF_GetError());
	exit(1);
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
	fprintf(stderr, "ERROR: SDL_CreateTextureFromSurface(): %s\n", SDL_GetError());
	exit(1);
    }

    SDL_Rect rect = {x, y, w, h};
    SDL_RenderCopy(renderer, texture, 0, &rect);
}

void menu_handle_input(SDL_Event* ev)
{
    if (ev->type == SDL_KEYDOWN && ev->key.repeat == 0) {
	switch (ev->key.keysym.scancode) {
	case SDL_SCANCODE_UP:
	    sound_play(SOUND_MENU_NAVIGATE);
	    if (selected <= 0) {
		selected = OPT_COUNT-1;
		break;
	    }
	    selected--;
	    break;
	case SDL_SCANCODE_DOWN:
	    sound_play(SOUND_MENU_NAVIGATE);
	    if (selected >= OPT_COUNT-1) {
		selected = 0;
		break;
	    }
	    selected++;
	    break;
	case SDL_SCANCODE_RETURN:
	    sound_play(SOUND_MENU_SELECT);
	    if (selected == OPT_START) {
		game_state = STATE_INGAME;
		break;
	    }
	    if (selected == OPT_QUIT) {
		SDL_Quit();
		exit(1);
	    }
	    break;
	}
    }
}

void menu_render(SDL_Renderer* renderer, GameState state)
{
    if (state == STATE_PAUSE) {
	options[0] = "Continue";
	SDL_Texture* transparent_back = SDL_CreateTexture(
	    renderer,
	    SDL_PIXELFORMAT_RGBA8888,
	    SDL_TEXTUREACCESS_TARGET,
	    SCREEN_WIDTH,
	    SCREEN_HEIGHT
	);
	SDL_SetTextureBlendMode(transparent_back, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 128, 128, 128, 128);
    } else {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    }
    SDL_RenderClear(renderer);

    TTF_Font* font = TTF_OpenFont("./res/font.ttf", 16);
    if (!font) {
	fprintf(stderr, "ERROR: TTF_OpenFont(): %s\n", TTF_GetError());
	exit(1);
    }

    menu_render_text(renderer, font, white, "The Maze", 15, 20, SCREEN_WIDTH - 20, (int)SCREEN_HEIGHT / 4);
    for (int i = 0; i < OPT_COUNT; i++) {
	SDL_Color col;
	if (i == selected) {
	    col = yellow;
	} else {
	    col = gray;
	}
#ifndef LARGE_SCREEN
	menu_render_text(renderer, font, col, options[i], 180, 160+90*i, SCREEN_WIDTH - 370, 50);	
#else
	menu_render_text(renderer, font, col, options[i], 180, 220+100*i, SCREEN_WIDTH - 370, 70);	
#endif // LARGE_SCREEN
    }
}
