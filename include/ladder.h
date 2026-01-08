#ifndef _LADDER_H_
#define _LADDER_H_

#include "./vec2.h"
#include "./ladder.h"
#include "./map.h"
#include "./sprite.h"
#include "./bitmap.h"

#define LADDER_CAP 20
#define LADDER_BITMAP_INDEX 26

typedef struct {
    Vec2 pos;
    MapType map1;
    MapType map2;
    Sprite sprite;
} Ladder;

extern Ladder g_ladders[LADDER_CAP];
extern int g_ladder_count;

void ladders_init(Bitmap* bitmap);
void ladder_create(MapType map1, MapType map2, Bitmap* bitmap, double x, double y);
void ladders_render(Screen* screen, Vec2* dir, Vec2* plane, Vec2* pos);
void ladders_update(Player* p);
void ladder_climb(Ladder* l, Player* p);

#endif // _LADDER_H_
