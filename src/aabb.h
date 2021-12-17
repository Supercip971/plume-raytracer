#ifndef AABB_H
#define AABB_H
#include <stddef.h>
#include "ray.h"
#include "vec3.h"

typedef struct aabb
{
    Vec3 min;
    Vec3 max;
} AABB;

typedef struct aabb_plane
{
    Vec3 normal;
    rt_float dist;
} AABBPlane;
AABB aabb_create(Vec3 min, Vec3 max);

#ifdef USE_NON_AVX_AABB
static inline bool aabb_hit(const AABB *self, const Ray *ray, rt_float t_min, rt_float t_max)
{
    size_t axis;
    rt_float invd, t0, t1;
    for (axis = 0; axis < 3; axis++)
    {
        invd = 1.0f / vec_axis(ray->direction, axis);
        t0 = (vec_axis(self->min, axis) - vec_axis(ray->origin, axis)) * invd;
        t1 = (vec_axis(self->max, axis) - vec_axis(ray->origin, axis)) * invd;

        if (invd < 0.0f)
        {
            rt_float _temp = t0;
            t0 = t1;
            t1 = _temp;
        }

        t_min = t0 > t_min ? t0 : t_min;
        t_max = t1 < t_max ? t1 : t_max;
        if (t_max <= t_min)
        {
            return false;
        }
    }

    return true;
}
#else
static inline bool aabb_hit(const AABB *self, const Ray *ray, rt_float t_min, rt_float t_max)
{

    Vec3 invd = vec3_div(vec3_create(1, 1, 1), ray->direction);
    Vec3 t0 = vec3_mul(vec3_sub(self->min, ray->origin), invd);
    Vec3 t1 = vec3_mul(vec3_sub(self->max, ray->origin), invd);

    if (invd.x < 0.0f)
    {
        rt_float _temp = t0.x;
        t0.x = t1.x;
        t1.x = _temp;
    }
    if (invd.y < 0.0f)
    {
        rt_float _temp = t0.y;
        t0.y = t1.y;
        t1.y = _temp;
    }
    if (invd.z < 0.0f)
    {
        rt_float _temp = t0.z;
        t0.z = t1.z;
        t1.z = _temp;
    }

    t_min = t0.x > t_min ? t0.x : t_min;
    t_max = t1.x < t_max ? t1.x : t_max;
    if (t_max <= t_min)
    {
        return false;
    }
    t_min = t0.y > t_min ? t0.y : t_min;
    t_max = t1.y < t_max ? t1.y : t_max;
    if (t_max <= t_min)
    {
        return false;
    }
    t_min = t0.z > t_min ? t0.z : t_min;
    t_max = t1.z < t_max ? t1.z : t_max;
    if (t_max <= t_min)
    {
        return false;
    }

    return true;
}
#endif

static inline bool aabb_intersect(const AABB *a, const AABB *b)
{
    return (a->min.x <= b->max.x && a->max.x >= b->min.x) &&
           (a->min.y <= b->max.y && a->max.y >= b->min.y) &&
           (a->min.z <= b->max.z && a->max.z >= b->min.z);
}

AABB aabb_surrounding(const AABB *a, const AABB *b);

static inline void get_aabb_plane(AABBPlane *result, const AABB *from)
{
    Vec3 center = vec3_add(from->min, vec3_mul_val(vec3_sub(from->max, from->min), 0.5));
    size_t i; 
    for ( i = 0; i < 3; i++)
    {
        result[i].dist = vec_axis(center, i);
        result[i].normal = vec3_create(0, 0, 0);
        *vec_axis_ptr(&result[i].normal, i) = 1.f;
    }
}

static inline Vec3 get_aabb_size(const AABB *self)
{
    return vec3_sub(self->max, self->min);
}

static inline rt_float aabb_ray_plane_dist(const Ray *r, const AABBPlane *plane)
{
    rt_float dot = vec3_dot(plane->normal, r->direction);
    if( dot < 0.0001f)
    {
        return 10000000000;
    }
    return (plane->dist - vec3_dot(plane->normal, r->origin)) / dot;
}

static inline bool aabb_contain(const AABB *self, Vec3 pos)
{
    return (self->min.x <= pos.x && self->max.x >= pos.x) &&
           (self->min.y <= pos.y && self->max.y >= pos.y) &&
           (self->min.z <= pos.z && self->max.z >= pos.z);
}
static inline Vec3 aabb_center(const AABB* self)
{
    Vec3 box_center = get_aabb_size(self);

    return vec3_add(self->min, vec3_mul_val(box_center, 0.5));
}
#endif
