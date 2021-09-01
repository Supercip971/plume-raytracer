#include "aabb.h"
#include <stdio.h>
#include "utils.h"

AABB aabb_create(Vec3 min, Vec3 max)
{
    AABB result;
    result.min = min;
    result.max = max;
    return result;
}

#ifdef DO_NOT_USE_PIXAR_AABB
static bool aabb_hit_dimension(rt_float minimum, rt_float maximum, rt_float origin, rt_float direction, rt_float *t_min, rt_float *t_max)
{
    rt_float t0, t1;
    rt_float min_interval = (minimum - origin) / direction;
    rt_float max_interval = (maximum - origin) / direction;
    t0 = fmin(min_interval, max_interval);
    t1 = fmax(min_interval, max_interval);

    *t_min = fmax(t0, *t_min);
    *t_max = fmin(t1, *t_max);
    if (t_max <= t_min)
    {
        return false;
    }
    return true;
}
#else
/*
static bool aabb_hit_dimension(rt_float minimum, rt_float maximum, rt_float origin, rt_float direction, rt_float *t_min, rt_float *t_max)
{
vec_axis
    rt_float invd;
    rt_float t0, t1;

    invd = 1.0 / direction;
    t0 = (minimum - origin) * invd;
    t1 = (maximum - origin) * invd;

    if (invd < 0.0f)
    {
        rt_float _temp = t0;
        t0 = t1;
        t1 = _temp;
    }

    *t_min = t0 > *t_min ? t0 : *t_min;
    *t_max = t1 < *t_max ? t1 : *t_max;
    if (t_max <= t_min)
    {
        return false;
    }
    return true;
}
*/
#endif

AABB aabb_surrounding(const AABB *__restrict a, const AABB *__restrict b)
{
    Vec3 min = vec3_create(rt_min(a->min.x, b->min.x), rt_min(a->min.y, b->min.y), rt_min(a->min.z, b->min.z));
    Vec3 max = vec3_create(rt_max(a->max.x, b->max.x), rt_max(a->max.y, b->max.y), rt_max(a->max.z, b->max.z));
    return aabb_create(min, max);
}
