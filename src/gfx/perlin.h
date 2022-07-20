#ifndef GFX_PERLIN_H
#define GFX_PERLIN_H

#include <math/vec3.h>
#include <utils/config.h>
#include <utils/utils.h>

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

rt_float trilinear_interp(rt_float c[2][2][2], rt_float u, rt_float v, rt_float w);

#endif
