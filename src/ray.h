#pragma once

#include "vec3.h"

typedef struct ray
{
    Vec3 origin;
    Vec3 direction;
} Ray;

static Vec3 ray_point_param(Ray self, float t)
{
    return vec3_add(self.origin, vec3_mul_val(self.direction, t));
}
