#pragma once
#include "../hitable.h"
#include "../ray.h"
#include "../vec3.h"
typedef struct moving_sphere
{
    rt_float radius;
    Vec3 pos_start, pos_end;
    rt_float time_start, time_end;
    Material self_material;
} MovingSphere;

bool hit_moving_sphere_object_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const MovingSphere *self);

Object moving_sphere_create(rt_float radius, rt_float time_start, rt_float time_end, Vec3 pos_start, Vec3 pos_end, Material sphere_material);
