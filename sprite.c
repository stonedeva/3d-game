#include "./sprite.h"
#include "./bitmap.h"
#include "./game.h"
#include <stdlib.h>
#include <SDL2/SDL_image.h>


Sprite sprites[SPRITE_CAP] = {0};
int sprite_count = 0;

Sprite sprite_load_from_path(char* file_path, double x, double y, int udiv, int vdiv, float vmove)
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

    Sprite sprite = {0};
    sprite.pos = (Vec2) {x, y};
    sprite.udiv = udiv;
    sprite.vdiv = vdiv;
    sprite.vmove = vmove;

    for (int y = 0; y < SPRITE_HEIGHT; y++) {
	for (int x = 0; x < SPRITE_WIDTH; x++) {
	    uint8_t* px = p + y * surface->pitch + x * surface->format->BytesPerPixel;
	    uint8_t r, g, b;
	    SDL_GetRGB(*(int*)px, surface->format, &r, &g, &b);
	    sprite.pixels[y * SPRITE_WIDTH + x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
	}
    }
    SDL_FreeSurface(surface);
    return sprite;
}

Sprite sprite_load_from_bitmap(Bitmap* bitmap, int bitmap_id, double x, double y, int udiv, int vdiv, float vmove)
{
    Sprite sprite = {0};
    sprite.pos = (Vec2) {x, y};
    sprite.udiv = udiv;
    sprite.vdiv = vdiv;
    sprite.vmove = vmove;
    
    for (int i = 0; i < SPRITE_WIDTH*SPRITE_HEIGHT; i++) {
	sprite.pixels[i] = bitmap->pixels[i][bitmap_id];
    }
    return sprite;
}

void sprite_render(Screen* screen, Sprite* sprite, Vec2* dir, Vec2* plane, Vec2* pos)
{
    double sp_x = sprite->pos.x - pos->x;
    double sp_y = sprite->pos.y - pos->y;

    double inv_det = 1.0 / (plane->x * dir->y - dir->x * plane->y); 
    double transf_x = inv_det * (dir->y * sp_x - dir->x * sp_y);
    double transf_y = inv_det * (-plane->y * sp_x + plane->x * sp_y);

    int vmove_screen = (int)(sprite->vmove / transf_y);
    int sp_screen_x = (int)((SCREEN_WIDTH / 2) * (1 + transf_x / transf_y));

    int sp_height = abs((int)(SCREEN_HEIGHT / transf_y)) / sprite->vdiv;
	
    int start_y = -sp_height / 2 + SCREEN_HEIGHT / 2 + vmove_screen;
    if (start_y < 0) {
	start_y = 0;
    }
    int end_y = sp_height / 2 + SCREEN_HEIGHT / 2 + vmove_screen;
    if (end_y >= SCREEN_HEIGHT) {
	end_y = SCREEN_HEIGHT - 1;
    }

    int sp_width = abs((int)(SCREEN_HEIGHT / transf_y)) / sprite->udiv;
    int start_x = -sp_width / 2 + sp_screen_x;
    if (start_x < 0) {
	start_x = 0;
    }
    int end_x = sp_width / 2 + sp_screen_x;
    if (end_x >= SCREEN_WIDTH) {
	end_x = SCREEN_WIDTH - 1;
    }

    for (int x = start_x; x < end_x; x++) {
	int texX = (int)(256 * (x - (-sp_width / 2 + sp_screen_x)) * TEX_WIDTH / sp_width) / 256;
	if (transf_y > 0 && x > 0 
	    && x < SCREEN_WIDTH 
	    && transf_y < zbuffer[x])
	for (int y = start_y; y < end_y; y++) {
	    int d = (y - vmove_screen) * 256 - SCREEN_HEIGHT * 128 + sp_height * 128; 
	    int texY = ((d * TEX_HEIGHT) / sp_height) / 256;

	    if (texX < 0 || texX >= TEX_WIDTH) continue;
	    if (texY < 0 || texY >= TEX_HEIGHT) continue;
	    
	    int color = sprite->pixels[TEX_WIDTH * texY + texX];
	    if (color != INVISIBLE_COLOR) {
		screen->pixels[y * SCREEN_WIDTH + x] = color;
	    }
	}
    }
}
