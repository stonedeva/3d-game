#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int main(void)
{
    SDL_Surface* surface = IMG_Load("../res/tile_colorpicker.png");
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

    printf("int map_colors[TILE_COUNT] = {\n");
    for (int x = 0; x < 11; x++) {
        uint8_t* px = p + x * surface->format->BytesPerPixel;
        uint8_t r, g, b;
        SDL_GetRGB(*(int*)px, surface->format, &r, &g, &b);

	int color = (0xFF << 24) | (r << 16) | (g << 8) | b;
	printf("    0x%02x,\n", color);
    }
    printf("}\n");
    SDL_FreeSurface(surface);
}
