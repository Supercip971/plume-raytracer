#ifndef TRANSLATE_H
#define TRANSLATE_H
#include <material/material.h>
#include <math/aabb.h>
#include <math/ray.h>
#include <math/vec3.h>
#include <shape/shape.h>

typedef struct translate
{
    Vec3 offset;
    Vec3 rotation;
    Object target;
} Translate;

Object translate(Object translated, Vec3 displacement, Vec3 rotation);
bool hit_translate_object_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const Translate *self);
bool translate_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const Translate *self);
bool translate_destroy(Translate *self);
#endif
