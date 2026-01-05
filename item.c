#include "./item.h"
#include "./vec2.h"

Item items[ITEM_CAP] = {0};
int item_count = 0;


void item_load(ItemType type, Bitmap* bitmap, MapType map, double x, double y)
{
    int bitmap_id = BITMAP_ITEM_OFFSET + type;
    Item i = {0};
    i.map = map;
    i.type = type;
    i.sprite = sprite_load_from_bitmap(bitmap, bitmap_id, x, y, 2, 2, SPRITE_GROUND_VALUE);
    items[item_count++] = i;
}

void item_load_with_scale(ItemType type, Bitmap* bitmap, MapType map, double x, double y,
			  int udiv, int vdiv, float vmove)
{
    int bitmap_id = BITMAP_ITEM_OFFSET + type;
    Item i = {0};
    i.map = map;
    i.type = type;
    i.sprite = sprite_load_from_bitmap(bitmap, bitmap_id, x, y, udiv, vdiv, vmove);
    items[item_count++] = i;
}

void items_init(Bitmap* bitmap)
{
    // MAP_CAVE
    item_load(ITEM_KEY, bitmap, MAP_CAVE, 10.0f, 8.0f);
    item_load(ITEM_TIMEKIT, bitmap, MAP_CAVE, 12.5f, 14.3f);
    item_load_with_scale(ITEM_LADDER, bitmap, MAP_CAVE, 3.5f, 1.6f, 1, 1, SPRITE_GROUND_VALUE);
    item_load(ITEM_KEY, bitmap, MAP_CAVE, 10.0f, 21.9f);

    // MAP_ICE
    item_load(ITEM_KEY, bitmap, MAP_ICE, 5.5f, 4.4f);
    item_load_with_scale(ITEM_LADDER, bitmap, MAP_ICE, 7.5f, 9.7f, 1, 1, SPRITE_GROUND_VALUE);
    item_load(ITEM_AXE, bitmap, MAP_ICE, 1.8f, 21.5f);
    item_load(ITEM_TIMEKIT, bitmap, MAP_ICE, 1.4f, 12.4f);

    // MAP_FIRE
    item_load(ITEM_TORCH, bitmap, MAP_FIRE, 6.5f, 5.9f);
    item_load(ITEM_KEY, bitmap, MAP_FIRE, 10.2f, 10.5f);
    item_load(ITEM_KEY, bitmap, MAP_FIRE, 1.5f, 22.3f);
    item_load(ITEM_KEY, bitmap, MAP_FIRE, 3.5f, 12.8f);
    item_load(ITEM_KEY, bitmap, MAP_FIRE, 3.5f, 18.3f);
    item_load(ITEM_KEY, bitmap, MAP_FIRE, 3.5f, 22.2f);
    item_load(ITEM_TIMEKIT, bitmap, MAP_FIRE, 10.8f, 3.6f);
    item_load(ITEM_FAKEKIT, bitmap, MAP_FIRE, 14.5f, 11.5f);
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
	if (items[i].type == ITEM_EMPTY || current_map_type != items[i].map) {
	    continue;
	}
	sprite_render(screen, &items[i].sprite, dir, plane, pos);
    }
}
