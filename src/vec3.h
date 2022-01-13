#ifndef VEC3_H
#define VEC3_H
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include "color.h"
#include "config.h"
#include "utils.h"

typedef struct vec3_t
{
    rt_float x;
    rt_float y;
    rt_float z;
    rt_float _n;
} Vec3;

static inline Vec3 vec3_create(rt_float x, rt_float y, rt_float z)
{
    Vec3 res;
    res.x = x;
    res.y = y;
    res.z = z;
    return res;
}

static inline Vec3 vec3_inv(Vec3 vec)
{
    return vec3_create(-vec.x, -vec.y, -vec.z);
}

static inline rt_float vec3_squared_length(Vec3 vec)
{
    return (vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

static inline rt_float vec3_length(Vec3 vec)
{
    return fast_sqrt(vec3_squared_length(vec));
}

static inline Vec3 vec3_add(Vec3 vec1, Vec3 vec2)
{
    return vec3_create(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z);
}

static inline Vec3 vec3_sub(Vec3 vec1, Vec3 vec2)
{
    return vec3_create(vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z);
}

#ifdef USE_INTRINSIC
#    include <immintrin.h>
#    include <x86intrin.h>

static inline Vec3 vec3_div(Vec3 vec1, Vec3 vec2)
{

    __m128 a = _mm_load_ps((float *)&vec1);
    __m128 b = _mm_load_ps((float *)&vec2);

    __m128 res_m = _mm_div_ps(a, b);
    Vec3 res = {};
    _mm_store_ps((float *)&res, res_m);

    return res;
}

static inline Vec3 vec3_mul(Vec3 vec1, Vec3 vec2)
{
    __m128 a = _mm_load_ps((float *)&vec1);
    __m128 b = _mm_load_ps((float *)&vec2);

    __m128 res_m = _mm_mul_ps(a, b);
    Vec3 res = {};
    _mm_store_ps((float *)&res, res_m);

    return res;
    return vec3_create(vec1.x * vec2.x, vec1.y * vec2.y, vec1.z * vec2.z);
}
#else
static inline Vec3 vec3_div(Vec3 vec1, Vec3 vec2)
{
    return vec3_create(vec1.x / vec2.x, vec1.y / vec2.y, vec1.z / vec2.z);
}

static inline Vec3 vec3_mul(Vec3 vec1, Vec3 vec2)
{
    return vec3_create(vec1.x * vec2.x, vec1.y * vec2.y, vec1.z * vec2.z);
}
#endif
static inline Vec3 vec3_mul_val(Vec3 vec1, rt_float x)
{
    return vec3_create(vec1.x * x, vec1.y * x, vec1.z * x);
}

static inline Vec3 vec3_div_val(Vec3 vec1, rt_float x)
{
    return vec3_create(vec1.x / x, vec1.y / x, vec1.z / x);
}

static inline rt_float vec3_dot(Vec3 vec1, Vec3 vec2)
{

    return (vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z);
}

static inline Vec3 vec3_cross(Vec3 vec1, Vec3 vec2)
{

    __m128 a = _mm_load_ps((float *)&vec1);
    __m128 b = _mm_load_ps((float *)&vec2);
    __m128 tmp0 = _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 tmp1 = _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 1, 0, 2));
    __m128 tmp2 = _mm_mul_ps(tmp0, b);
    __m128 tmp3 = _mm_mul_ps(tmp0, tmp1);
    __m128 tmp4 = _mm_shuffle_ps(tmp2, tmp2, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 res_m = _mm_sub_ps(tmp3, tmp4);
    Vec3 res = {};
    _mm_store_ps((float *)&res, res_m);

    return res;
}

static inline bool is_vec3_near_zero(Vec3 vec)
{
    const rt_float precision = 1e-8;

    return (fabs((double)vec.x) < precision) && (fabs((double)vec.y) < precision) && (fabs((double)vec.z) < precision);
}

/* vec / sqrt(vecx * vecx + vecy * vecy + vecz * vecz)
// vec * 1/sqrt()
// vec * Q_rdqrt()*/
static inline Vec3 vec3_unit(Vec3 vec)
{
    return vec3_mul_val(vec, Q_rsqrt(vec3_squared_length(vec)));
}

/* todo: implement way better random_vec3_x because this doesn't look good */

static inline Vec3 random_vec3_unit(void)
{
    while (true)
    {
        Vec3 p = vec3_create(random_rt_float() * 2 - 1, random_rt_float() * 2 - 1, random_rt_float() * 2 - 1);
        if (vec3_squared_length(p) >= 1)
            continue;
        return p;
    }
}

static inline Vec3 random_vec3_unit_in_disk(void)
{
    while (true)
    {
        Vec3 p = vec3_create(random_rt_float() * 2 - 1, random_rt_float() * 2 - 1, 0);
        if (vec3_squared_length(p) >= 1)
            continue;
        return p;
    }
}

static inline Vec3 random_vec3_in_hemisphere(Vec3 normal)
{
    Vec3 in_unit = random_vec3_unit();

    if (vec3_dot(in_unit, normal) > 0.0)
    {
        return in_unit;
    }

    return vec3_inv(in_unit);
}

static inline Vec3 reflect(Vec3 vec1, Vec3 vec2)
{
    return vec3_sub(vec1, vec3_mul_val(vec2, (vec3_dot(vec1, vec2) * 2.0)));
}

static inline Vec3 refract(Vec3 vec1, Vec3 vec2, rt_float ni_over_nt)
{
    rt_float cos_theta = rt_min(vec3_dot(vec3_inv(vec1), vec2), 1.0);
    Vec3 r_out_perp = vec3_mul_val(vec3_add(vec1, vec3_mul_val(vec2, cos_theta)), ni_over_nt);
    Vec3 r_out_parl = vec3_mul_val(vec2, -fast_sqrt(rt_abs((1.0 - vec3_squared_length(r_out_perp)))));

    return vec3_add(r_out_perp, r_out_parl);
}

static inline Vec3 vec_from_color(Color col)
{
    return vec3_create(col.r, col.g, col.b);
}

static inline Color vec_to_color(Vec3 from)
{
    return color_create(
        rt_min(from.x, 1),
        rt_min(from.y, 1),
        rt_min(from.z, 1));
}

/* maybe the attribute hot is totally useless ? todo: check if it is really necessary */

__attribute__((hot)) static inline rt_float vec_axis(Vec3 vec, int axis)
{
    if (axis == 0)
    {
        return vec.x;
    }
    else if (axis == 1)
    {
        return vec.y;
    }
    return vec.z;
}

__attribute__((hot)) static inline rt_float *vec_axis_ptr(Vec3 *vec, int axis)
{
    if (axis == 0)
    {
        return &vec->x;
    }
    else if (axis == 1)
    {
        return &vec->y;
    }
    return &vec->z;
}
static inline void get_sphere_uv(const Vec3 *point, rt_float *__restrict u, rt_float *__restrict v)
{
    rt_float theta = fast_acos(-(point->y));
    rt_float phi = fast_atan2(-(point->z), point->x);

    *u = phi * (M_2_PI) + 0.5;
    *v = theta * M_1_PI;
}

static inline Vec3 random_vec3_cosine_direction(void)
{
    rt_float r1 = random_rt_float();
    rt_float r2 = random_rt_float();
    rt_float z = fast_sqrt(1.0 - r2);

    rt_float phi = r1 * M_PI * 2;
    rt_float x = fast_cos(phi) * fast_sqrt(r2);
    rt_float y = fast_sin(phi) * fast_sqrt(r2);

    return vec3_create(x, y, z);
}

#endif
