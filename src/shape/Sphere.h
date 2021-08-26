#pragma once
#include "../hitable.h"
#include "../ray.h"
#include "../vec3.h"
typedef struct sphere
{
    rt_float radius;
    Vec3 pos;
    AABB self_bounding_box; /* put here as we need to calculate it once */
    Material self_material;
} Sphere;

bool hit_sphere_object_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const Sphere *self);

Object sphere_create(rt_float radius, Vec3 pos, Material sphere_material);
