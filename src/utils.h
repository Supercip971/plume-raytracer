#pragma once
#include <stdint.h>
#include <stdlib.h>

uint32_t fast_rand(void);

#define random_double() \
    (double)((double)fast_rand() / (double)0x7FFF)

float Q_rsqrt(float number);

float fast_sqrt(float number);
