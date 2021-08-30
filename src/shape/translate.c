
#include "translate.h"

static FLATTEN bool box_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const Translate *self)
{
    if (self->target.get_aabb(time_start, time_end, output, self->target.data))
    {
        output->max = vec3_add(output->max, self->offset);
        output->min = vec3_add(output->min, self->offset);
    }

    return true;
}

static FLATTEN bool hit_translate_object_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const Translate *self)
{
    Ray moved = ray;

    moved.origin = vec3_sub(moved.origin, self->offset);

    if (!self->target.collide(moved, t_min, t_max, record, self->target.data))
    {
        return false;
    }

    record->pos = vec3_add(record->pos, self->offset);
    set_face_normal(&moved, record->normal, record);
    return true;
}
Object translate(Object translated, Vec3 displacement, Vec3 rotation)
{
    Object res;
    Translate *b = malloc(sizeof(Translate));

    b->target = translated;
    b->offset = displacement;
    b->rotation = rotation;

    res.data = b;
    res.get_aabb = (ObjectGetAABB)box_get_aabb;
    res.destroy = (ObjectDestroy)translated.destroy;
    res.collide = (ObjectCallback)hit_translate_object_callback;
    res.is_leaf = true;

    return res;
}
