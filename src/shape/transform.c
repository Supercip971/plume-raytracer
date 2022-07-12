#include "transform.h"
#include <stdlib.h>
#include "matrix4x4.h"

static AABB get_transformed_aabb(AABB from, Matrix4x4 matrix)
{
    Vec3 points[8];
    Vec3 min, max;
    int i;

    points[0] = from.min;
    points[1] = vec3_add(from.min, vec3_create(from.max.x, 0, 0));
    points[2] = vec3_add(from.min, vec3_create(0, from.max.y, 0));
    points[3] = vec3_add(from.min, vec3_create(from.max.x, from.max.y, 0));
    points[4] = vec3_add(from.min, vec3_create(0, 0, from.max.z));
    points[5] = vec3_add(from.min, vec3_create(from.max.x, 0, from.max.z));
    points[6] = vec3_add(from.min, vec3_create(0, from.max.y, from.max.z));
    points[7] = from.max;

    for (i = 0; i < 8; i++)
    {
        matrix_apply_point(&matrix, &points[i]);
    }
    min = points[0];
    max = points[7];
    for (i = 0; i < 8; i++)
    {
        min = vec3_min(min, points[i]);
        max = vec3_max(max, points[i]);
    }

    return aabb_create(min, max);
}

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

    b->transformed_aabb = get_transformed_aabb(b->transformed_aabb, matrix);
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
    Vec3 res;
    matrix_apply_point(&self->inv_matrix, &origin);
    res = object_random(origin, &self->target);
//    matrix_apply_vector(&self->matrix, &res);

    return res;
}
