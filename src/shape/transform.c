#include "transform.h"

#include <shape/hittable.h>
#include <stdlib.h>
#include <math/matrix4x4.h>
#include <shape/shape.h>

static AABB get_transformed_aabb(AABB from, Matrix4x4 matrix)
{
    Vec3 points[8] = {
        [0] = from.min,
        [1] = vec3_add(from.min, vec3_create(from.max.x, 0, 0)),
        [2] = vec3_add(from.min, vec3_create(0, from.max.y, 0)),
        [3] = vec3_add(from.min, vec3_create(from.max.x, from.max.y, 0)),
        [4] = vec3_add(from.min, vec3_create(0, 0, from.max.z)),
        [5] = vec3_add(from.min, vec3_create(from.max.x, 0, from.max.z)),
        [6] = vec3_add(from.min, vec3_create(0, from.max.y, from.max.z)),
        [7] = from.max,
    };

    for (int i = 0; i < 8; i++)
    {
        matrix_apply_point(&matrix, &points[i]);
    }

    Vec3 min = points[0];
    Vec3 max = points[7];
    for (int i = 0; i < 8; i++)
    {
        min = vec3_min(min, points[i]);
        max = vec3_max(max, points[i]);
    }

    return aabb_create(min, max);
}

Object transform(Object translated, Matrix4x4 matrix)
{
    Transform *b = malloc(sizeof(Transform));

    b->target = translated;
    b->matrix = matrix;
    matrix_inverse(&matrix, &b->inv_matrix);
    object_get_aabb(-1000, 1000, &b->transformed_aabb, &translated);

    b->transformed_aabb = get_transformed_aabb(b->transformed_aabb, matrix);

    Object res = {
        .data = b,
        .type = SHAPE_TRANSFORM,
        .is_leaf = true,
    };

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

    matrix_apply_vector(&self->matrix, &record->outward);
    set_face_normal(&ray, record->outward, record);

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
    matrix_apply_point(&self->inv_matrix, &origin);

    Vec3 res = object_random(origin, &self->target);
    //    matrix_apply_vector(&self->matrix, &res);

    return res;
}
