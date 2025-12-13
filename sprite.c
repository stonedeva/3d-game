#include "./sprite.h"
#include <stdlib.h>
#include <SDL2/SDL_image.h>

void sprite_load(SpriteManager* manager, char* file_path, double x, double y)
{
    SDL_Surface* surface = IMG_Load(file_path);
    if (!surface) {
	fprintf(stderr, "sprite_load(): Could not load sprite\n");
	exit(1);
    }
    uint8_t* p = (uint8_t*)surface->pixels;
    if (surface->format->BitsPerPixel != 24) {
	fprintf(stderr, "BitsPerPixel != 24: Could not read image file\n");
	SDL_FreeSurface(surface);
	exit(1);
    }

    GameSprite sprite = {0};
    for (int y = 0; y < SPRITE_HEIGHT; y++) {
	for (int x = 0; x < SPRITE_WIDTH; x++) {
	    uint8_t* px = p + y * surface->pitch + x * surface->format->BytesPerPixel;
	    uint8_t r, g, b;
	    SDL_GetRGB(*(int*)px, surface->format, &r, &g, &b);
	    sprite.pixels[y * SPRITE_WIDTH + x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
	}
    }
    SDL_FreeSurface(surface);

    if (manager->sprite_count >= SPRITE_CAP) {
	fprintf(stderr, "SPRITE_CAP has been reached!\n");
	exit(1);
    }
    manager->sprites[manager->sprite_count++] = sprite;
}
