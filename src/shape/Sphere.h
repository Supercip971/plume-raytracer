#pragma once
#include "../hitable.h"
#include "../vec3.h"
#include "../ray.h"
typedef struct sphere
{
    rt_float radius;
    Vec3 pos;
} Sphere;

bool hit_sphere_object_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const Sphere *self);

Object sphere_create(rt_float radius, Vec3 pos);
