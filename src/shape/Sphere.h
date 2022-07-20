#pragma once

#include <material/material.h>
#include <math/ray.h>
#include <math/vec3.h>
#include <shape/shape.h>

typedef struct sphere
{
    rt_float radius;
    rt_float radius_squared;
    rt_float radius_inv;
    Vec3 pos;
    AABB self_bounding_box; /* put here as we need to calculate it once */
    Material self_material;
} Sphere;

bool hit_sphere_object_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const Sphere *self);

Object sphere_create(rt_float radius, Vec3 pos, Material sphere_material);

bool sphere_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const Sphere *self);

rt_float sphere_pdf_value(Vec3 origin, Vec3 direction, const Sphere *self);

Vec3 sphere_random(Vec3 origin, const Sphere *self);
