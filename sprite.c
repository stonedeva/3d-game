#include "./sprite.h"
#include "./bitmap.h"
#include "./game.h"
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

    Sprite sprite = {0};
    sprite.pos = (Vec2) {x, y};

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
/*
void sprite_render(Screen* screen, SpriteManager* manager, Vec2* pos, Vec2* dir, Vec2* plane)
{
    printf("0x%02x\n", manager->sprites[0].pixels[0]);
    for (int i = 0; i < manager->sprite_count; i++) {
        Sprite* sprite = &manager->sprites[i];

        // Calculate the sprite's position relative to the player's position and direction
        float sprite_x = sprite->pos.x - pos->x;
        float sprite_y = sprite->pos.y - pos->y;

        // Transform the sprite's position to camera space
        float inv_det = 1.0 / (plane->x * dir->y - dir->x * plane->y);
        float transform_x = inv_det * (dir->y * sprite_x - dir->x * sprite_y);
        float transform_y = inv_det * (-plane->y * sprite_x + plane->x * sprite_y);

        // Calculate the screen x-coordinate for the sprite
        int sprite_screen_x = (int)((SCREEN_WIDTH / 2) * (1 + transform_x / transform_y));

        // Calculate the height of the sprite on the screen
        int sprite_height = abs((int)(SCREEN_HEIGHT / transform_y));

        // Calculate the y-coordinates where the sprite will be drawn
        int draw_start_y = -sprite_height / 2 + SCREEN_HEIGHT / 2;
        if (draw_start_y < 0) draw_start_y = 0;
        int draw_end_y = sprite_height / 2 + SCREEN_HEIGHT / 2;
        if (draw_end_y >= SCREEN_HEIGHT) draw_end_y = SCREEN_HEIGHT - 1;

        // Calculate the width of the sprite on the screen
        int sprite_width = abs((int)(SCREEN_HEIGHT / transform_y));
        int draw_start_x = -sprite_width / 2 + sprite_screen_x;
        if (draw_start_x < 0) draw_start_x = 0;
        int draw_end_x = sprite_width / 2 + sprite_screen_x;
        if (draw_end_x >= SCREEN_WIDTH) draw_end_x = SCREEN_WIDTH - 1;

        for (int stripe = draw_start_x; stripe < draw_end_x; stripe++) {
            int tex_x = (int)((stripe - (-sprite_width / 2 + sprite_screen_x)) * TEX_WIDTH / sprite_width);
            if (transform_y > 0 && stripe >= 0 && stripe < SCREEN_WIDTH) {
                for (int y = draw_start_y; y < draw_end_y; y++) {
                    int tex_y = (int)((y - draw_start_y) * TEX_HEIGHT / sprite_height);
                    int color = sprite->pixels[tex_y * TEX_WIDTH + tex_x];

                    if (color != 0) {  // Skip transparent pixels
                        screen->pixels[y * SCREEN_WIDTH + stripe] = color;
                    }
                }
            }
        }
    }
}*/


void sprite_render(Screen* screen, SpriteManager* mgr, Vec2* pos, Vec2* dir, Vec2* plane)
{
    for (int i = 0; i < mgr->sprite_count; i++)
    {
	Sprite sprite = mgr->sprites[i];
	double sp_x = sprite.pos.x - pos->x;
	double sp_y = sprite.pos.y - pos->y;

	double inv_det = 1.0 / (plane->x * dir->y - dir->x * plane->y); 
	double transf_x = inv_det * (dir->y * sp_x - dir->x * sp_y);
	double transf_y = inv_det * (-plane->y * sp_x + plane->x * sp_y);

	int sp_screen_x = (int)((SCREEN_WIDTH / 2) * (1 + transf_x / transf_y));

	int sp_height = abs((int)(SCREEN_HEIGHT / transf_y));
	
	int start_y = -sp_height / 2 + SCREEN_HEIGHT / 2;
	if (start_y < 0) start_y = 0;
	int end_y = sp_height / 2 + SCREEN_HEIGHT / 2;
	if (end_y >= SCREEN_HEIGHT) end_y = SCREEN_HEIGHT - 1;

	int sp_width = abs((int)(SCREEN_HEIGHT / transf_y));
	int start_x = -sp_width / 2 + sp_screen_x;
	if (start_x < 0) start_x = 0;
	int end_x = sp_width / 2 + sp_screen_x;
	if (end_x >= SCREEN_WIDTH) end_x = SCREEN_WIDTH - 1;

	for (int stripe = start_x; stripe < end_x; stripe++) {
	    int texX = (int)(256 * (stripe - (-sp_width / 2 + sp_screen_x)) * TEX_WIDTH / sp_width) / 256;
	    if (transf_y > 0 && stripe > 0 
		&& stripe < SCREEN_WIDTH 
		&& transf_y < zbuffer[stripe])
	    for (int y = start_y; y < end_y; y++) {
		int d = (y) * 256 - SCREEN_HEIGHT * 128 + sp_height * 128; 
		int texY = ((d * TEX_HEIGHT) / sp_height) / 256;
		if (texX < 0 || texX >= TEX_WIDTH) continue;
		if (texY < 0 || texY >= TEX_HEIGHT) continue;
		int color = mgr->sprites[i].pixels[TEX_WIDTH * texY + texX];
		if (color != INVISIBLE_COLOR) {
		    screen->pixels[y * SCREEN_WIDTH + stripe] = color;
		}
	    }
        }
    }
}
