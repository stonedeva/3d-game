#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "./game.h"
#include "./map.h"
#include "./player.h"
#include "./screen.h"
#include "./sound.h"
#include "./sprite.h"
#include "./item.h"
#include "./menu.h"
#include "./ladder.h"

#ifdef TARGET_BROWSER
#include <emscripten/emscripten.h>
#endif // TARGET_BROWSER

int tick_count = 0;
int pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
int game_timer = 20*60;

// Forward declaration
Player player;
Screen screen;
GameState game_state = STATE_MENU;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* texture;


void update(void)
{
    player_update(&player);
    ladders_update(&player);
    items_update(&player);
    tick_count++;
}

void render(SDL_Renderer* renderer, SDL_Texture* texture)
{
    switch (game_state) {
    case STATE_MENU:
    case STATE_PAUSE:
    case STATE_VICTORY:
    case STATE_GAMEOVER:
	menu_render(renderer);
	break;
    case STATE_INGAME:
	screen_render(&screen, &player.dir, &player.plane, &player.pos);
	SDL_UpdateTexture(texture, 0, pixels, SCREEN_WIDTH * sizeof(int));
	SDL_RenderCopy(renderer, texture, 0, 0);
	break;
    }
    SDL_RenderPresent(renderer);
}

void main_loop(void)
{
    SDL_Event ev;
    int running = 1;

    const double ns_per_tick = 1000000000.0 / TARGET_FPS;

    uint64_t last_time = SDL_GetPerformanceCounter();
    double delta = 0.0;
    int frames = 0;

    uint32_t timer = SDL_GetTicks();

    while (running) {
	while (SDL_PollEvent(&ev) != 0) {
	    if (ev.type == SDL_QUIT) {
		running = 0;
		return;
	    }
	    if (ev.key.keysym.sym == SDLK_ESCAPE && game_state == STATE_INGAME) {
		game_state = STATE_PAUSE;
	    }
	    menu_handle_input(&ev);
	}

	uint64_t now = SDL_GetPerformanceCounter();
	double passed = (double)(now - last_time) * (1000000000.0 / SDL_GetPerformanceFrequency());
	last_time = now;
	delta += passed / ns_per_tick;

	while (delta >= 1.0) {
	    update();
	    delta -= 1.0;
	}

	render(renderer, texture);
	frames++;

	if (SDL_GetTicks() - timer >= 1000) {
	    if (game_timer == 0) {
		player_game_over(&player);
	    }
	    if (game_state == STATE_INGAME) {
		game_timer--;
	    }

	    char win_title[29];
	    snprintf(win_title, sizeof(win_title), "The Maze | FPS %d | Ticks %d",
		    frames, tick_count);
	    SDL_SetWindowTitle(window, win_title);
	    printf("px: %f, py: %f\n", player.pos.x, player.pos.y);
	    frames = 0;
	    tick_count = 0;
	    timer += 1000;
	}

	SDL_Delay(1000 / TARGET_FPS);
    }
}

int init_sdl_libs(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
	fprintf(stderr, "SDL_Init(): Could not init video: %s\n", SDL_GetError());
	return 1;
    }
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
	fprintf(stderr, "SDL_Init(): Could not init audio: %s\n", SDL_GetError());
	SDL_Quit();
	return 1;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
	fprintf(stderr, "IMG_Init(): Could not init image: %s\n", IMG_GetError());
	SDL_Quit();
	return 1;
    }
    if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == 0) {
	fprintf(stderr, "Mix_Init(): Could not init mixer: %s\n", Mix_GetError());
	SDL_Quit();
	return 1;
    }
    if (TTF_Init() < 0) {
	fprintf(stderr, "TTF_Init(): Could not init ttf: %s\n", TTF_GetError());
	SDL_Quit();
	return 1;
    }
    return 0;
}

void quit(void)
{
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void game_reset(void)
{
    game_state = STATE_INGAME;
    current_map_type = MAP_CAVE;
    player = player_init();
}

int init(void)
{
    if (init_sdl_libs() == 1) {
	return 1;
    }
    SDL_ShowCursor(SDL_DISABLE);

    window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, 
						  SDL_WINDOWPOS_UNDEFINED,
						  SCREEN_WIDTH, SCREEN_HEIGHT, 
						  SDL_WINDOW_SHOWN);
    if (!window) {
	fprintf(stderr, "SDL_CreateWindow(): Could not create window: %s\n", SDL_GetError());
	SDL_Quit();
	return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
	fprintf(stderr, "SDL_CreateRenderer(): Could not create renderer: %s\n", 
		SDL_GetError());
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 1;
    }

    texture = SDL_CreateTexture(
	renderer,
	SDL_PIXELFORMAT_ARGB8888,
	SDL_TEXTUREACCESS_STREAMING,
	SCREEN_WIDTH,
	SCREEN_HEIGHT
    );
    if (!texture) {
	fprintf(stderr, "SDL_CreateTexture(): Could not create texture: %s\n", SDL_GetError());
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 1;
    }
    menu_init();

    player = player_init();
    screen = screen_init(pixels);
    sound_init();
    items_init(&screen.bitmap);
    ladders_init(&screen.bitmap);

    map_load_from_png("./res/maps/cave.png");

    main_loop();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}

int main(int argc, char** argv)
{
    int res = init();
    sound_cleanup();
    quit();
    return res;
}
