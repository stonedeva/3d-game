#ifndef _ITEM_H_
#define _ITEM_H_

#include "./sprite.h"
#include "./bitmap.h"
#include "./player.h"
#include "./map.h"

#define ITEM_CAP 20

typedef enum {
    ITEM_EMPTY = 0,
    ITEM_KEY,
    ITEM_TIMEKIT,
    ITEM_LADDER, // FIXME: Ladder is no longer an item but keep for now to offset correctly
    ITEM_AXE,
    ITEM_TORCH,
    ITEM_FAKEKIT,
    ITEM_GOAL
} ItemType;

typedef struct {
    MapType map;
    ItemType type;
    Sprite sprite;
} Item;


extern Item items[ITEM_CAP];
extern int item_count;

void item_load(ItemType type, Bitmap* bitmap, MapType map, double x, double y);
void item_load_with_scale(ItemType type, Bitmap* bitmap, MapType map, double x, double y,
			  int udiv, int vdiv, float vmove);
void items_init(Bitmap* bitmap);
void items_update(Player* p);
void items_render(Screen* screen, Vec2* dir, Vec2* plane, Vec2* pos);

#endif // _ITEM_H_
