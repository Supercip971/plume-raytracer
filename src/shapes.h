#pragma once
#include "boolean.h"
#include "hitable.h"
#include "ray.h"
#include "vec3.h"
typedef struct sphere
{
    float radius;
    Vec3 pos;
} Sphere;

bool hit_sphere_object_callback(Ray ray, float t_min, float t_max, HitRecord *record, const Sphere *self);

Sphere *sphere_create(float radius, Vec3 pos);
