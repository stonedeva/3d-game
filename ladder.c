#include "./ladder.h"
#include "./map.h"

Ladder ladders[LADDER_CAP] = {0};
int ladder_count = 0;


void ladder_create(MapType map1, MapType map2, Bitmap* bitmap, double x, double y)
{
    if (ladder_count >= LADDER_CAP) {
	return;
    }
    Ladder l = {0};
    l.map1 = map1;
    l.map2 = map2;
    l.pos = (Vec2) {x,y};
    l.sprite = sprite_load_from_bitmap(bitmap, LADDER_BITMAP_INDEX, x, y, 1, 1, SPRITE_GROUND_VALUE);
    
    ladders[ladder_count++] = l;
}

void ladders_init(Bitmap* bitmap)
{
    ladder_create(MAP_CAVE, MAP_ICE, bitmap, 3.5f, 1.6f);
    ladder_create(MAP_ICE, MAP_FIRE, bitmap, 7.5f, 9.2f);
}

void ladders_render(Screen* screen, Vec2* dir, Vec2* plane, Vec2* pos)
{
    for (int i = 0; i < ladder_count; i++) {
	Ladder l = ladders[i];
	if (current_map_type != l.map1 && current_map_type != l.map2)
	    continue;
	sprite_render(screen, &ladders[i].sprite, dir, plane, pos);
    }
}

void ladders_update(Player* p)
{
    for (int i = 0; i < ladder_count; i++) {
	if (current_map_type != ladders[i].map1 && current_map_type != ladders[i].map2)
	    continue;
	if (player_check_collision(p, &ladders[i].sprite, 0.1f)) {
	    ladder_climb(&ladders[i], p);
	}
    }
}

void ladder_climb(Ladder* l, Player* p)
{
    double lx = l->pos.x;
    double ly = l->pos.y;
    if (lx < 0 || lx >= MAP_WIDTH || ly < 0 || ly >= MAP_HEIGHT)
	return;

    if (current_map_type == l->map1) {
	map_switch(p, l->map2);
    } else {
	map_switch(p, l->map1);
    }
}
