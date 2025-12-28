#ifndef _ENTITY_H_
#define _ENTITIY_H_

#define ENTITY_CAP 20
#define ENTITY_ENGAGE_DIST 10

#include "./sprite.h"
#include "./player.h"
#include "./vec2.h"

typedef enum {
    ENTITY_SKELETON = 0,
    ENTITY_COUNT
} EntityType;

typedef enum {
    ENTITY_STATE_IDLE = 0,
    ENTITY_STATE_ENGAGING
} EntityState;

typedef struct {
    EntityType type;
    Sprite sprite;
    int health;
} Entity;

extern Entity entities[ENTITY_CAP];
extern int entity_count;


void entity_init();
void entities_render(Screen* screen, Vec2* dir, Vec2* plane, Vec2* pos);
void entities_update(Player* p);
void entity_load(EntityType type, double x, double y);

#endif // _ENTITY_H_
