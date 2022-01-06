#include "transform.h"
#include <stdlib.h>

Object transform(Object translated, Matrix4x4 matrix)
{
    Object res;
    Transform *b = malloc(sizeof(Transform));

    b->target = translated;
    b->matrix = matrix;
    matrix_inverse(&matrix, &b->inv_matrix);

    res.data = b;
    res.type = SHAPE_TRANSFORM;
    res.is_leaf = true;

    return res;
}
bool hit_transform_object_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const Transform *self)
{
    Ray moved = ray;

    matrix_apply_ray(&self->inv_matrix, &moved);

    if (!object_collide(moved, t_min, t_max, record, (Object *)&self->target))
    {
        return false;
    }
    matrix_apply_point(&self->matrix, &record->pos);
    set_face_normal(&moved, record->normal, record);
    return true;
}
bool transform_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const Transform *self)
{
    if (object_get_aabb(time_start, time_end, output, (Object *)&self->target))
    {

        matrix_apply_point(&self->matrix, &output->min);
        matrix_apply_point(&self->matrix, &output->max);
    }

    return true;
}
bool transform_destroy(Transform *self)
{
    return object_destroy(&self->target);
}
