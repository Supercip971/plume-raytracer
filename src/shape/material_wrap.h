#ifndef MATERIAL_WRAP_H
#define MATERIAL_WRAP_H
#include <shape/shape.h>
#include <material/material.h>
#include <math/matrix4x4.h>
#include <shape/hittable.h>

typedef struct
{
    Object target;
    Material material;
} MaterialWrap;

rt_float mwrap_pdf_value(Vec3 origin, Vec3 direction, const MaterialWrap *self);

Vec3 mwrap_pdf_random(Vec3 origin, const MaterialWrap *self);

Object material_wrap(Object target, Material material);

static inline bool hit_mwrap_object_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const MaterialWrap *self)
{
    bool r = object_collide(ray, t_min, t_max, record, (Object*)&self->target);
    if(r)
    {
        record->material = self->material;
    }
    return r;
}

static inline bool mwrap_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const MaterialWrap *self)
{
    return object_get_aabb(time_start, time_end, output, (Object*)&self->target);
}

static inline bool mwrap_destroy(MaterialWrap *self)
{
    return object_destroy((Object*)self);
}

#endif /* C95E0A6A_3BD4_4ED1_89B9_E4D0699C094C */
