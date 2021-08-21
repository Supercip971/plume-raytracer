#pragma once
#include "boolean.h"
#include "ray.h"
#include "vec3.h"
typedef struct sphere
{
    float radius;
    Vec3 pos;
} Sphere;

float ray_sphere_collide(Ray ray, Sphere sphere);
