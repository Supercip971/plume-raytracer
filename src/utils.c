#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "vec3.h"

static uint32_t g_seed = 0xfffaa;

uint32_t fast_rand(void)
{
    g_seed = (214013 * g_seed + 2531011);
    return (g_seed >> 16) & 0x7FFF;
}

/* note: quake inv squareroot is slower than sse square root (+ only for floats)*/

#ifdef USE_QUAKE_INV_SQUAREROOT

rt_float Q_rsqrt(rt_float number)
{
    long i = 0;
    rt_float x2, y = 0;
    const rt_float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y = number;
    memcpy(&i, &y, sizeof(rt_float));
    /* i = *(long *)&y;*/
    i = 0x5f3759df - (i >> 1);
    memcpy(&y, &i, sizeof(rt_float));
    /*  y = *(rt_float *)&i;*/
    y = y * (threehalfs - (x2 * y * y));
    y = y * (threehalfs - (x2 * y * y)); /* 2nd iteration, this can be removed*/

    return y;
}

#else

#endif

void get_sphere_uv(const Vec3 *point, rt_float *u, rt_float *v)
{
    rt_float theta = acos(-(point->y));
    rt_float phi = atan2(-(point->z), point->x) + M_PI;

    *u = phi / (2 * M_PI);
    *v = theta / M_PI;
}
