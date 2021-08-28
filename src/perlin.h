#pragma once

#include "config.h"
#include "utils.h"
#include "vec3.h"

typedef struct perlin_noise
{
    int *perm_x;
    int *perm_y;
    int *perm_z;
    rt_float *ran_float;
    size_t point_count;
    Vec3 *ranvec;
} PerlinNoise;

PerlinNoise noise_create(void);

rt_float noise(const Vec3 *point, const PerlinNoise *self);

rt_float turb(const Vec3 *point, int depth, const PerlinNoise *self);
