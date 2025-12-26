#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <math.h>
#include <stdlib.h>
#include "./vec2.h"
#include "./bitmap.h"
#include "./game.h"

#define FOV 60

typedef struct {
    int* pixels;
    Bitmap bitmap;
} Screen;

typedef struct {
    Vec2 dir;
    Vec2 side_dist;
    Vec2 delta_dist;
    double perp_wall_dist;
    int side;
} Ray;

extern double zbuffer[SCREEN_WIDTH];


Screen screen_init(int* pixels);
void screen_render_map(Screen* screen, Vec2* dir, Vec2* plane, Vec2* pos);
void screen_render_floor(Screen* screen, Vec2* dir, Vec2* plane, Vec2* pos);
void screen_render(Screen* screen, Vec2* dir, Vec2* plane, Vec2* pos);

void screen_load_texture(Screen* screen, char* file_path);

#endif // _SCREEN_H_
