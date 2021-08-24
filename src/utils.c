#include "utils.h"

static uint32_t g_seed = 0xfffaa;
uint32_t fast_rand(void)
{

    g_seed = (214013 * g_seed + 2531011);
    return (g_seed >> 16) & 0x7FFF;
}

float Q_rsqrt(float number)
{
    long i = 0;
    float x2, y = 0;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y = number;
    i = *(long *)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float *)&i;
    y = y * (threehalfs - (x2 * y * y));
    /*	y = y * ( threehalfs - ( x2 * y * y ) )*; // 2nd iteration, this can be removed*/

    return y;
}
