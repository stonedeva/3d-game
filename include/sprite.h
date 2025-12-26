#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <stddef.h>
#include "./screen.h"
#include "./vec2.h"

#define SPRITE_WIDTH 16
#define SPRITE_HEIGHT 16
#define SPRITE_CAP 10
#define SPRITE_GROUND_VALUE 64

typedef struct {
    int pixels[SPRITE_WIDTH * SPRITE_HEIGHT];
    Vec2 pos;
    int udiv;
    int vdiv;
    float vmove;
} Sprite;

typedef struct {
    Sprite sprites[SPRITE_CAP];
    int sprite_count;
} SpriteManager;

extern Sprite sprites[SPRITE_CAP];
extern int sprite_count;


void sprite_load(char* file_path, double x, double y, int udiv, int vdiv, float vmove);
void sprite_render(Screen* screen, Sprite* sprite, Vec2* pos, Vec2* dir, Vec2* plane);

#endif // _SPRITE_H_
