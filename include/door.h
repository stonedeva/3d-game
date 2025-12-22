#ifndef _DOOR_H_
#define _DOOR_H_

#define DOOR_TILE 2

typedef enum {
    IDLE, CLOSING, OPENING
} DoorState;

typedef struct {
    DoorState state;
    float open; // 0 -> 1
} Door;

Door doors[24][24]; // TODO: use MAP_WIDTH, MAP_HEIGHT instead of fix2

#endif // _DOOR_H_
