#pragma once
#include <stdint.h>
#include <stdlib.h>
#include "config.h"

uint32_t fast_rand(void);

#define random_rt_float() \
    (rt_float)((rt_float)fast_rand() / (rt_float)0x7FFF)

float Q_rsqrt(float number);

float fast_sqrt(float number);

#define rt_float_max (rt_float)((uint64_t)-1)

#define FLATTEN __attribute__((flatten))

#define rt_min(a, b) (((a) < (b)) ? (a) : (b))
#define rt_max(a, b) (((a) > (b)) ? (a) : (b))
