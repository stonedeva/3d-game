#pragma once

#include <math.h>

typedef struct {
    double x;
    double y;
} Vec2;

double vec2_get_dist(Vec2* v1, Vec2* v2, double radius);
