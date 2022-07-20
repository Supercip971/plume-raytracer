#ifndef AABB_H
#define AABB_H

#include <math/ray.h>
#include <math/vec3.h>
#include <stddef.h>

typedef struct aabb
{
    Vec3 min;
    Vec3 max;
} AABB;

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
static inline bool aabb_hit2(const AABB *self, const Ray *ray, rt_float t_min, rt_float t_max)
{
    Vec3 v_max = vec3_create(100000000, 100000000, 100000000);
    Vec3 v_min = vec3_create(-100000000, -100000000, -100000000);
    Vec3 inv_d = ray->inv_dir;

    __m128 a_min = _mm_load_ps(v_min.v);
    __m128 a_max = _mm_load_ps(v_max.v);

    __m128 b_min = _mm_load_ps(self->min.v);
    __m128 b_max = _mm_load_ps(self->max.v);
    __m128 pos = _mm_load_ps(ray->origin.v);

    __m128 inv_dir = _mm_load_ps(inv_d.v);

    __m128 l_min_in = _mm_mul_ps(_mm_sub_ps(b_min, pos), inv_dir);
    __m128 l_max_in = _mm_mul_ps(_mm_sub_ps(b_max, pos), inv_dir);

    __m128 fl_min_a = _mm_min_ps(l_min_in, a_min);
    __m128 fl_max_a = _mm_min_ps(l_max_in, a_max);

    __m128 fl_min_b = _mm_max_ps(l_min_in, b_min);
    __m128 fl_max_b = _mm_max_ps(l_max_in, b_max);

    __m128 l_max = _mm_max_ps(fl_max_a, fl_max_b);
    __m128 l_min = _mm_min_ps(fl_min_a, fl_min_b);

    __m128 lmax0 = _mm_shuffle_ps(l_max, l_max, 0x39);
    __m128 lmin0 = _mm_shuffle_ps(l_min, l_min, 0x39);

    l_max = _mm_min_ss(lmax0, l_max);
    l_min = _mm_max_ss(lmin0, l_min);

    __m128 lmax1 = _mm_movehl_ps(l_max, l_max);
    __m128 lmin1 = _mm_movehl_ps(l_min, l_min);

    l_max = _mm_min_ss(lmax1, l_max);
    l_min = _mm_max_ss(lmin1, l_min);

    rt_float val_max = 1000;
    rt_float val_min = 1000;

    _mm_store_ss(&val_max, l_max);
    _mm_store_ss(&val_min, l_min);

    if (val_min <= t_min || val_max >= t_max)
    {
        return false;
    }

    return _mm_comige_ss(l_max, _mm_setzero_ps()) & _mm_comige_ss(l_max, l_min);
}
static inline bool aabb_hit(const AABB *self, const Ray *ray, rt_float t_min, rt_float t_max)
{
    (void)t_min;
    (void)t_max;

    Vec3 t0 = vec3_mul(vec3_sub(self->min, ray->origin), ray->inv_dir);
    Vec3 t1 = vec3_mul(vec3_sub(self->max, ray->origin), ray->inv_dir);
    Vec3 tmax = vec3_max(t0, t1);
    Vec3 tmin = vec3_min(t0, t1);

    return vec3_min_comp(tmax) >= vec3_max_comp(tmin);
}

static inline bool aabb_hit3(const AABB *self, const Ray *ray, rt_float t_min, rt_float t_max)
{

    Vec3 invd = vec3_div(vec3_create(1, 1, 1), ray->direction);
    Vec3 t0 = vec3_mul(vec3_sub(self->min, ray->origin), invd);
    Vec3 t1 = vec3_mul(vec3_sub(self->max, ray->origin), invd);

    if (invd.x < 0.0f)
    {
        // if(dir.x < 0)
        // swap(t0.x, t1.x);
        rt_float _temp = t0.x;
        t0.x = t1.x;
        t1.x = _temp;
    }
    if (invd.y < 0.0f)
    {
        //  if(dir.y < 0)
        // swap(t0.y, t1.y);
        rt_float _temp = t0.y;
        t0.y = t1.y;
        t1.y = _temp;
    }
    if (invd.z < 0.0f)
    {
        // if(dir.z < 0)
        // swap(t0.z, t1.z);
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

static inline rt_float aabb_distance_squarred(const AABB *box, Vec3 point)
{
    Vec3 d = vec3_max(vec3_sub(box->min, point), vec3_sub(point, box->max));

    return (vec3_squared_length(d));
}

static inline rt_float aabb_distance(const AABB *box, Vec3 point)
{
    return fast_sqrt(aabb_distance_squarred(box, point));
}

static inline bool aabb_intersect(const AABB *a, const AABB *b)
{
    return (a->min.x <= b->max.x && a->max.x >= b->min.x) &&
           (a->min.y <= b->max.y && a->max.y >= b->min.y) &&
           (a->min.z <= b->max.z && a->max.z >= b->min.z);
}

AABB aabb_surrounding(const AABB *a, const AABB *b);
#endif
