#ifndef UTILS_H
#define UTILS_H
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "config.h"

#define rt_float_max (rt_float)((uint64_t)-1)

#define FLATTEN

#define rt_min(a, b) (((a) < (b)) ? (a) : (b))
#define rt_max(a, b) (((a) > (b)) ? (a) : (b))
#define rt_abs(a) (((a) < 0) ? -(a) : (a))
#define rt_floor(a) ((a == (long)a || a >= 0) ? (rt_float)((long)a) : (rt_float)((long)a - 1))

#define rt_clamp(a, from, to) rt_max(rt_min(a, to), from)

#define RAD2DEG(x) (x) * (180.0f / M_PI)
#define DEG2RAD(x) (x) * (M_PI / 180.0f)

static inline uint32_t fast_rand(void)
{
    static uint32_t shuffle_table[4] = {
        2419050644,
        343247216,
    };
    uint32_t s1 = shuffle_table[0];
    uint32_t s0 = shuffle_table[1];
    uint32_t result = s0 + s1;
    shuffle_table[0] = s0;
    s1 ^= s1 << 11;
    shuffle_table[1] = s1 ^ s0 ^ (s1 >> 9) ^ (s0 >> 5);
    return (uint32_t)result;
}

#define rt_copysign(x, y) (__builtin_copysign(x, y))

static inline float Q_rsqrt(float number);

static inline float fast_sqrt(float number);

#define random_rt_float() \
    (rt_float)((rt_float)fast_rand() / (rt_float)4294967296)

#define random_rt_range(a, b) \
    (random_rt_float() * ((b) - (a)) + (a))
/* you may want to implement here the fastest implementation */

#ifdef USE_INTRINSIC

#    include <immintrin.h>
#    include <x86intrin.h>

static inline float fast_sqrt(float number)
{
    float a;
    float b = number;
    __m128 in = _mm_load_ss(&b);
    _mm_store_ss(&a, _mm_sqrt_ss(in));
    return a;
}

static inline float Q_rsqrt(float number)
{
    float a;
    float b = number;
    __m128 in = _mm_load_ss(&b);
    _mm_store_ss(&a, _mm_rsqrt_ss(in));
    return a;
}

#else

static inline float fast_sqrt(float number)
{
    return sqrt(number);
}

static inline float Q_rsqrt(float number)
{
    return 1 / sqrt(number);
}

#endif
static inline rt_float fast_atan2(rt_float y, rt_float x)
{
    rt_float abs_y = rt_abs(y) + 0.0000000001;
    rt_float r = (x - rt_copysign(abs_y, x)) / (abs_y + rt_abs(x));
    rt_float angle = (M_PI_2)-rt_copysign(M_PI_4, x);

    angle += (0.1963f * r * r - 0.9817f) * r;
    return rt_copysign(angle, y);
}

static inline rt_float fast_acos(rt_float x)
{
    return (-0.69813170079773212 * x * x - 0.87266462599716477) * x + 1.5707963267948966;
}

static inline rt_float fast_sin(rt_float x)
{
    x *= M_1_PI * 0.5;
    x -= 0.25 + rt_floor(x + 0.25);
    x *= 16 * (rt_abs(x) - (0.5));
    return x;
}
static inline rt_float fast_cos(rt_float x)
{
    return fast_sin(M_PI_2 + x);
}

void init_sin_table(void);
#endif
