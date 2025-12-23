#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <math.h>
#include <stdlib.h>
#include "./vec2.h"

#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define IMG_WIDTH 16
#define IMG_HEIGHT 16
#define FOV 60
#define TEXTURE_CAP 5

typedef struct {
    char* file_path;
    int pixels[IMG_WIDTH * IMG_HEIGHT];
} GameImage;

typedef struct {
    int* pixels;
    double camera_height;
    // first element is celling/floor texture
    GameImage textures[TEXTURE_CAP];
    size_t texture_count;
    GameImage wall_tex;
    GameImage ground_tex;
    GameImage door_tex;
} Screen;

static int screen_map[MAP_WIDTH][MAP_HEIGHT] = {
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,1,1,1,0,0,0,1},
  {1,0,0,0,0,1,0,0,0,0,0,1,0,1,0,1,1,0,0,1,1,0,0,1},
  {1,0,0,0,1,0,0,0,0,0,0,2,0,0,1,0,0,0,0,0,1,0,0,1},
  {1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,1,1,0,0,1,0,0,1},
  {1,0,0,0,0,1,1,0,1,1,1,0,1,0,0,1,0,1,0,0,1,0,0,1},
  {1,0,0,0,0,0,1,0,1,0,0,0,0,1,0,1,0,0,0,0,1,0,0,1},
  {1,0,0,0,0,0,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,0,0,1},
  {1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
  {1,0,0,0,0,0,1,1,1,1,1,0,0,1,1,1,1,1,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,1,2,0,1,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};


Screen screen_init(int* pixels);
void screen_render_map(Screen* screen, Vec2* dir, Vec2* plane, Vec2* pos);
void screen_render_floor(Screen* screen, Vec2* dir, Vec2* plane, Vec2* pos);
void screen_render(Screen* screen, Vec2* dir, Vec2* plane, Vec2* pos);

void screen_load_texture(Screen* screen, char* file_path);

#endif // _SCREEN_H_
