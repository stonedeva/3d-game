#ifndef _ITEM_H_
#define _ITEM_H_

#include "./sprite.h"
#include "./bitmap.h"
#include "./player.h"

#define ITEM_CAP 20

typedef enum {
    ITEM_EMPTY = 0,
    ITEM_KEY,
    ITEM_MEDKIT,
    ITEM_SPEEDKIT
} ItemType;

typedef struct {
    ItemType type;
    Sprite sprite;
} Item;


extern Item items[ITEM_CAP];
extern int item_count;

void item_load(ItemType type, Bitmap* bitmap, double x, double y);
void items_init(Bitmap* bitmap);
void items_render(Screen* screen, Vec2* dir, Vec2* plane, Vec2* pos);

#endif // _ITEM_H_
