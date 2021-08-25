#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <x86intrin.h>
#include <xmmintrin.h>
static uint32_t g_seed = 0xfffaa;

uint32_t fast_rand(void)
{
    g_seed = (214013 * g_seed + 2531011);
    return (g_seed >> 16) & 0x7FFF;
}

/* note: quake squareroot is slower than sse square root */

#ifdef USE_QUAKE_INV_SQUAREROOT

float Q_rsqrt(float number)
{
    long i = 0;
    float x2, y = 0;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y = number;
    memcpy(&i, &y, sizeof(float));
    /* i = *(long *)&y;*/
    i = 0x5f3759df - (i >> 1);
    memcpy(&y, &i, sizeof(float));
    /*  y = *(float *)&i;*/
    y = y * (threehalfs - (x2 * y * y));
    y = y * (threehalfs - (x2 * y * y)); /* 2nd iteration, this can be removed*/

    return y;
}

#else

float Q_rsqrt(float number)
{
    float a;
    float b = number;
    __m128 in = _mm_load_ss(&b);
    _mm_store_ss(&a, _mm_rsqrt_ss(in));
    return a;
}

#endif

float fast_sqrt(float number)
{
    float a;
    float b = number;
    __m128 in = _mm_load_ss(&b);
    _mm_store_ss(&a, _mm_sqrt_ss(in));
    return a;
}
