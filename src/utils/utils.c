#include "utils.h"
#include <math.h>
#include <math/vec3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

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
