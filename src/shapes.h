#pragma once
#include <stdbool.h>
#include "ray.h"
#include "vec3.h"

typedef struct sphere
{
    float radius;
    Vec3 pos;
} Sphere;

bool ray_sphere_collide(Ray ray, Sphere sphere);
