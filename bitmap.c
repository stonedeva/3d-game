#include "./bitmap.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdint.h>
#include <assert.h>


Bitmap bitmap_load(char* file_path)
{
    Bitmap bitmap = {0};

    SDL_Surface* surface = IMG_Load(file_path);
    if (!surface) {
        fprintf(stderr, "IMG_Load(): Could not read image file: %s\n",
		SDL_GetError());
        exit(1);
    }
    if (SDL_MUSTLOCK(surface)) {
	if (SDL_LockSurface(surface) != 0) {
	    fprintf(stderr, "SDL_LockSurface(): Could not lock surface: %s\n",
		    SDL_GetError());
	    exit(1);
	}
    }

    uint8_t* pixels = (uint8_t*)surface->pixels;

    /* 8 bit for r,g,b (0-255 each) */
    /*
    if (surface->format->BitsPerPixel != 24) {
        fprintf(stderr, "BitsPerPixel != 24: Could not read image file\n");
        SDL_FreeSurface(surface);
        exit(1);
    }*/

    int bpp = surface->format->BytesPerPixel;
    for (int i = 0; i < TEX_COUNT; i++) {
	int tx = (i % 8) * TEX_WIDTH;
	int ty = (i / 8) * TEX_HEIGHT;
	for (int y = 0; y < TEX_HEIGHT; y++) {
	    for (int x = 0; x < TEX_WIDTH; x++) {
		uint8_t* px = pixels + (ty+y) * surface->pitch + (tx+x) * bpp;
		uint8_t r, g, b;
		SDL_GetRGB(*(int*)px, surface->format, &r, &g, &b);
		bitmap.pixels[y * TEX_WIDTH + x][i] = (0xFF << 24) | (r << 16) | (g << 8) | b;
	    }
	}
    }

    if (SDL_MUSTLOCK(surface)) {
	SDL_UnlockSurface(surface);
    }

    SDL_FreeSurface(surface);
    return bitmap;
}

