#include "./vec2.h"

double vec2_get_dist(Vec2* v1, Vec2* v2, double radius)
{
    double dx = v1->x - v2->x;
    double dy = v1->y - v2->y;
    return (dx*dx + dy*dy) < (radius * radius);
}

Vec2 vec2_cross(Vec2* v1, Vec2* v2)
{
    return (Vec2) {v1->x * v2->x, v1->y * v2->y};
}
