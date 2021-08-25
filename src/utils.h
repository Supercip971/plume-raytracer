#pragma once
#include <stdint.h>
#include <stdlib.h>

uint32_t fast_rand(void);

#define random_double() \
    (double)((double)fast_rand() / (double)0x7FFF)

float Q_rsqrt(float number);

float fast_sqrt(float number);

#define double_max (double)((uint64_t)-1)

#define FLATTEN __attribute__((flatten))
