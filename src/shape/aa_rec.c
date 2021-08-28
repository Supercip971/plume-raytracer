#include "aa_rec.h"
#include <stdlib.h>
#include "../utils.h"

static FLATTEN bool hit_aarect_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const AARect *self)
{
    rt_float t;
    rt_float x, y;
    Vec3 outward;

    t = (self->k - ray.origin.z) / (ray.direction.z);

    if (t < t_min || t > t_max)
    {
        return false;
    }

    x = ray.origin.x + t * ray.direction.x;
    y = ray.origin.y + t * ray.direction.y;

    if (x < self->x_0 || x > self->x_1 || y < self->y_0 || y > self->y_1)
    {
        return false;
    }

    record->u = (x - self->x_0) / (self->x_1 - self->x_0);
    record->v = (y - self->y_0) / (self->y_1 - self->y_0);
    record->t = t;

    outward = vec3_create(0, 0, 1);
    set_face_normal(&ray, outward, record);

    record->material = self->self_material;
    record->pos = ray_point_param(ray, t);

    return true;
}

static FLATTEN bool aarect_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const AARect *self)
{
    (void)time_start;
    (void)time_end;
    *output = self->self_box;

    return true;
}

Object aarect_create(rt_float x_0, rt_float x_1, rt_float y_0, rt_float y_1, rt_float k, Material rect_material)
{
    Object result;
    AARect *rect = malloc(sizeof(AARect));
    rect->self_material = rect_material;
    rect->self_box = aabb_create(vec3_create(x_0, y_0, k - 0.0001), vec3_create(x_1, y_1, k + 0.0001));

    rect->k = k;

    rect->x_0 = x_0;
    rect->x_1 = x_1;
    rect->y_0 = y_0;
    rect->y_1 = y_1;

    result.collide = (ObjectCallback)hit_aarect_callback;
    result.get_aabb = (ObjectGetAABB)aarect_get_aabb;
    result.data = rect;
    result.destroy = NULL;
    result.is_leaf = true;

    return result;
}
