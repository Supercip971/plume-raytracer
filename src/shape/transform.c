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

    object_get_aabb(-1000, 1000, &b->transformed_aabb, &translated);
    matrix_apply_point(&matrix, &b->transformed_aabb.min);
    matrix_apply_point(&matrix, &b->transformed_aabb.max);

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
    (void)time_start;
    (void)time_end;
    *output = self->transformed_aabb;

    return true;
}
bool transform_destroy(Transform *self)
{
    return object_destroy(&self->target);
}
rt_float transform_pdf_value(Vec3 origin, Vec3 direction, const Transform *self)
{
    Vec3 origin_c = origin;
    Vec3 direction_c = direction;
    matrix_apply_point(&self->inv_matrix, &origin_c);
    matrix_apply_vector(&self->inv_matrix, &direction_c);

    return object_pdf_value(origin_c, direction_c, &self->target);
}
Vec3 transform_pdf_random(Vec3 origin, const Transform *self)
{
    Vec3 res;

    matrix_apply_point(&self->inv_matrix, &origin);
    res = object_random(origin, &self->target);
    matrix_apply_vector(&self->matrix, &res);

    return res;
}
