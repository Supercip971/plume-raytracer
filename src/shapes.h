#pragma once
#include "config.h"
#include "boolean.h"
#include "hitable.h"
#include "material.h"
#include "ray.h"
#include "vec3.h"
typedef struct sphere
{
    rt_float radius;
    Vec3 pos;
} Sphere;

bool hit_sphere_object_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const Sphere *self);

Sphere *sphere_create(rt_float radius, Vec3 pos);
