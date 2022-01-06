#ifndef TRANSLATE_H
#define TRANSLATE_H
#include "../aabb.h"
#include "../hitable.h"
#include "../ray.h"
#include "../vec3.h"

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
