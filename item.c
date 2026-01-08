#include "./item.h"
#include "./vec2.h"

Item g_items[ITEM_CAP] = {0};
int g_item_count = 0;


void item_load(ItemType type, Bitmap* bitmap, MapType map, double x, double y)
{
    int bitmap_id = BITMAP_ITEM_OFFSET + type;
    Item i = {0};
    i.map = map;
    i.type = type;
    i.sprite = sprite_load_from_bitmap(bitmap, bitmap_id, x, y, 2, 2, SPRITE_GROUND_VALUE);
    g_items[g_item_count++] = i;
}

void item_load_with_scale(ItemType type, Bitmap* bitmap, MapType map, double x, double y,
			  int udiv, int vdiv, float vmove)
{
    int bitmap_id = BITMAP_ITEM_OFFSET + type;
    Item i = {0};
    i.map = map;
    i.type = type;
    i.sprite = sprite_load_from_bitmap(bitmap, bitmap_id, x, y, udiv, vdiv, vmove);
    g_items[g_item_count++] = i;
}

void items_init(Bitmap* bitmap)
{
    // MAP_CAVE
    item_load(ITEM_KEY, bitmap, MAP_CAVE, 10.0f, 8.0f);
    item_load(ITEM_TIMEKIT, bitmap, MAP_CAVE, 12.5f, 14.3f);
    item_load(ITEM_KEY, bitmap, MAP_CAVE, 10.0f, 21.9f);

    // MAP_ICE
    item_load(ITEM_KEY, bitmap, MAP_ICE, 5.5f, 4.4f);
    item_load(ITEM_KEY, bitmap, MAP_ICE, 19.5f, 14.8f);
    item_load(ITEM_AXE, bitmap, MAP_ICE, 1.8f, 21.5f);
    item_load(ITEM_TIMEKIT, bitmap, MAP_ICE, 1.4f, 12.4f);

    // MAP_FIRE
    item_load(ITEM_TORCH, bitmap, MAP_FIRE, 6.5f, 5.9f);
    item_load(ITEM_KEY, bitmap, MAP_FIRE, 10.2f, 10.5f);
    item_load(ITEM_KEY, bitmap, MAP_FIRE, 1.5f, 22.3f);
    item_load(ITEM_KEY, bitmap, MAP_FIRE, 3.5f, 12.8f);
    item_load(ITEM_KEY, bitmap, MAP_FIRE, 3.5f, 18.3f);
    item_load(ITEM_KEY, bitmap, MAP_FIRE, 3.5f, 22.2f);
    item_load(ITEM_KEY, bitmap, MAP_FIRE, 11.6f, 15.5f);
    item_load(ITEM_TIMEKIT, bitmap, MAP_FIRE, 10.8f, 3.6f);
    item_load(ITEM_FAKEKIT, bitmap, MAP_FIRE, 14.5f, 11.5f);
}

void items_update(Player* p)
{
    for (int i = 0; i < g_item_count; i++) {
	if (g_items[i].type == ITEM_EMPTY) {
	    continue;
	}
	if (player_check_collision(p, &g_items[i].sprite, 0.5f)) {
	    player_pickup_item(p, i);
	}
    }
}

void items_render(Screen* screen, Vec2* dir, Vec2* plane, Vec2* pos)
{
    for (int i = 0; i < g_item_count; i++) {
	if (g_items[i].type == ITEM_EMPTY || g_current_map_type != g_items[i].map) {
	    continue;
	}
	sprite_render(screen, &g_items[i].sprite, dir, plane, pos);
    }
}
