#ifndef UTILS_H
#define UTILS_H
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "config.h"

#define rt_float_max (rt_float)((uint64_t)-1)

#define FLATTEN __attribute__((flatten))

#define rt_min(a, b) (((a) < (b)) ? (a) : (b))
#define rt_max(a, b) (((a) > (b)) ? (a) : (b))
#define rt_abs(a) (((a) < 0) ? -(a) : (a))

#define rt_clamp(a, from, to) rt_max(rt_min(a, to), from)
uint32_t fast_rand(void);

#define random_rt_float() \
    (rt_float)((rt_float)fast_rand() / (rt_float)0x7FFF)

static inline float Q_rsqrt(float number);

static inline float fast_sqrt(float number);

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
#endif
