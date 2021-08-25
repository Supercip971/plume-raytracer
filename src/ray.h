#pragma once

#include "config.h"
#include "vec3.h"

typedef struct ray
{
    Vec3 origin;
    Vec3 direction;
    rt_float time; /* using double for more precision (todo: check if we can use float) */
} Ray;

Vec3 ray_point_param(Ray self, rt_float t);
