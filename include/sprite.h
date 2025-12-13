#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <stddef.h>

#define SPRITE_WIDTH 32
#define SPRITE_HEIGHT 32
#define SPRITE_CAP 10

typedef struct {
    int pixels[SPRITE_WIDTH * SPRITE_HEIGHT];
    double x;
    double y;
} GameSprite;

typedef struct {
    GameSprite sprites[SPRITE_CAP];
    int sprite_count;
} SpriteManager;

void sprite_load(SpriteManager* manager, char* file_path, double x, double y);

#endif // _SPRITE_H_
