
#include "translate.h"
#include <shape/hittable.h>
#include <shape/shape.h>
FLATTEN bool translate_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const Translate *self)
{
    if (object_get_aabb(time_start, time_end, output, (Object *)&self->target))
    {
        output->max = vec3_add(output->max, self->offset);
        output->min = vec3_add(output->min, self->offset);
    }

    return true;
}

FLATTEN bool hit_translate_object_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const Translate *self)
{
    Ray moved = ray;

    moved.origin = vec3_sub(moved.origin, self->offset);

    if (!object_collide(moved, t_min, t_max, record, (Object *)&self->target))
    {
        return false;
    }

    record->pos = vec3_add(record->pos, self->offset);

    set_face_normal(&moved, record->normal, record);
    return true;
}

bool translate_destroy(Translate *self)
{
    return object_destroy(&self->target);
}

Object translate(Object translated, Vec3 displacement, Vec3 rotation)
{
    Translate *b = malloc(sizeof(Translate));

    *b = (Translate){
        .target = translated,
        .offset = displacement,
        .rotation = rotation,
    };

    Object res = {
        .data = b,
        .type = SHAPE_TRANSLATE,
    };

    return res;
}
