#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "./game.h"
#include "./player.h"
#include "./screen.h"

int tick_count = 0;
int pixels[SCREEN_WIDTH * SCREEN_HEIGHT];

Player player = (Player) {.px = 22, 
			  .py = 12,
			  .pa = 1.5,
			  .dir_x = -1,
			  .dir_y = 0,
			  .plane_x = 0,
			  .plane_y = 0.66
			 };
Screen screen = (Screen) {0};


void update(void)
{
    player_handle_input(&screen, &player);
    tick_count++;
}

void render(SDL_Renderer* renderer, SDL_Texture* texture)
{
    memset(pixels, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(int));
    screen_render_map(&screen, &player);

    SDL_UpdateTexture(texture, 0, pixels, SCREEN_WIDTH * sizeof(int));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, 0, 0);
    SDL_RenderPresent(renderer);
}

void run_loop(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture)
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
	    if (ev.type == SDL_QUIT || (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE)) {
		running = 0;
		return;
	    }

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
	    printf("FPS: %d, Ticks: %d\n", frames, tick_count);
	    frames = 0;
	    tick_count = 0;
	    timer += 1000;
	}

	SDL_Delay(1000 / TARGET_FPS);
    }
}

int main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
	fprintf(stderr, "SDL_Init(): Could not init sdl: %s\n", SDL_GetError());
	return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
						  SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
	fprintf(stderr, "SDL_CreateWindow(): Could not create window: %s\n", SDL_GetError());
	SDL_Quit();
	return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
	fprintf(stderr, "SDL_CreateRenderer(): Could not create renderer: %s\n", SDL_GetError());
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 1;
    }

    SDL_Texture* texture = SDL_CreateTexture(
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

    screen = screen_init(pixels);

    run_loop(window, renderer, texture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
