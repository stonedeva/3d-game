#include "./entity.h"
#include "./player.h"


Entity entities[ENTITY_CAP] = {0};
int entity_count = 0;

char* entity_res[ENTITY_COUNT] = {
    [ENTITY_SKELETON] = "./res/skeleton.png",
};

int entity_health[ENTITY_COUNT] = {
    [ENTITY_SKELETON] = 5
};

float entity_speed[ENTITY_COUNT] = {
    [ENTITY_SKELETON] = 0.05f
};

void entity_load(EntityType type, double x, double y)
{
    Entity e = {0};
    e.type = type;
    e.sprite = sprite_load_from_path(entity_res[type], x, y, 1, 1, 0);
    e.health = entity_health[type];
    entities[entity_count++] = e;
}

void entities_render(Screen* screen, Vec2* dir, Vec2* plane, Vec2* pos)
{
    for (int i = 0; i < entity_count; i++) {
	sprite_render(screen, &entities[i].sprite, dir, plane, pos);
    }
}

void entity_attack_player(Entity* e, Player* p)
{
    int dx = (int)p->pos.x - e->sprite.pos.x;
    int dy = (int)p->pos.y - e->sprite.pos.y;

    bool is_behind = 0;
    bool is_left = 0;
    if (dy < 0) {
	is_behind = 1;
    }
    if (dx < 0) {
	is_left = 0;
    }
    dx = abs(dx);
    dy = abs(dy);

    if (dx < ENTITY_ENGAGE_DIST || dy < ENTITY_ENGAGE_DIST) {
	e->sprite.pos.y += entity_speed[e->type];
    }
}

void entities_update(Player* p)
{
    for (int i = 0; i < entity_count; i++) {
	Entity e = entities[i];
	if (player_check_collision(p, &e.sprite, 0.5f)) {
	    player_take_damage(p, 1);
	}
    }
}

void entity_init()
{
    entity_load(ENTITY_SKELETON, 18.5f, 12.0f);
}
