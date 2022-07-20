#pragma once

#include <shape/shape.h>
#include <material/material.h>
#include <math/ray.h>
#include <math/vec3.h>

typedef struct triangle
{
    Vec3 posa;
    Vec3 posb;
    Vec3 posc;

    Vec2 uva;
    Vec2 uvb;
    Vec2 uvc;
} Triangle;

bool hit_triangle_object_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const Triangle *self);

Object triangle_create(Vec3 posa, Vec3 posb, Vec3 posc);

bool triangle_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const Triangle *self);

rt_float triangle_pdf_value(Vec3 origin, Vec3 direction, const Triangle *self);

Vec3 triangle_random(Vec3 origin, const Triangle *self);

