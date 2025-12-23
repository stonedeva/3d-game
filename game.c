#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "./game.h"
#include "./player.h"
#include "./screen.h"

int tick_count = 0;
int pixels[SCREEN_WIDTH * SCREEN_HEIGHT];

// Forward declaration
Player player;
Screen screen;


void update(void)
{
    player_handle_input(&player);
    tick_count++;
}

void render(SDL_Renderer* renderer, SDL_Texture* texture)
{
    screen_render(&screen, &player.dir, &player.plane, &player.pos);

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
	    printf("px: %f, py: %f\n", player.pos.x, player.pos.y);
	    frames = 0;
	    tick_count = 0;
	    timer += 1000;
	}

	SDL_Delay(1000 / TARGET_FPS);
    }
}

void map_load_from_file(char* file_path)
{
    FILE* fp = fopen(file_path, "rb");
    if (!fp) {
	fprintf(stderr, "ERROR: fopen(): Failed to open map file: %s\n",
		strerror(errno));
	exit(1);
    }

    size_t n = fread(screen_map, sizeof(int), MAP_WIDTH*MAP_HEIGHT, fp);
    if (n != MAP_WIDTH*MAP_HEIGHT) {
	fprintf(stderr, "ERROR: fread(): Expected %d bytes read %d bytes: %s\n",
		n, MAP_WIDTH*MAP_HEIGHT, strerror(errno));
	exit(1);
    }
    fclose(fp);
}

int init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
	fprintf(stderr, "SDL_Init(): Could not init sdl: %s\n", SDL_GetError());
	return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
	fprintf(stderr, "IMG_Init(): Could not init image: %s\n", IMG_GetError());
	return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, 
						  SDL_WINDOWPOS_UNDEFINED,
						  SCREEN_WIDTH, SCREEN_HEIGHT, 
						  SDL_WINDOW_SHOWN);
    if (!window) {
	fprintf(stderr, "SDL_CreateWindow(): Could not create window: %s\n", SDL_GetError());
	SDL_Quit();
	return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
	fprintf(stderr, "SDL_CreateRenderer(): Could not create renderer: %s\n", 
		SDL_GetError());
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

    player = player_init();
    screen = screen_init(pixels);

    run_loop(window, renderer, texture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

int main(int argc, char** argv)
{
    if (argc >= 2) {
	char* map_path = argv[1];
	map_load_from_file(map_path);
    }

    return init();
}
