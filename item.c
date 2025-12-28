#include "./item.h"
#include "./vec2.h"

Item items[ITEM_CAP] = {0};
int item_count = 0;


void item_load(ItemType type, Bitmap* bitmap, int bitmap_id, double x, double y)
{
    bitmap_id += BITMAP_ITEM_OFFSET;
    Item i = {0};
    i.type = type;
    i.sprite = sprite_load_from_bitmap(bitmap, bitmap_id, x, y, 2, 2, SPRITE_GROUND_VALUE);
    items[item_count++] = i;
}

void items_init(Bitmap* bitmap)
{
    item_load(ITEM_KEY, bitmap, ITEM_KEY, 10.0f, 8.0f);
    item_load(ITEM_MEDKIT, bitmap, ITEM_MEDKIT, 12.0f, 14.5f);
}

void items_update(Player* p)
{
    for (int i = 0; i < item_count; i++) {
	if (items[i].type == ITEM_EMPTY) {
	    continue;
	}
	if (player_check_collision(p, &items[i].sprite, 0.5f)) {
	    player_pickup_item(p, i);
	}
    }
}

void items_render(Screen* screen, Vec2* dir, Vec2* plane, Vec2* pos)
{
    for (int i = 0; i < item_count; i++) {
	if (items[i].type == ITEM_EMPTY) {
	    continue;
	}
	sprite_render(screen, &items[i].sprite, dir, plane, pos);
    }
}
