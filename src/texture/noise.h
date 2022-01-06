#pragma once
#include "../perlin.h"
#include "texture.h"

typedef struct perlin
{
    PerlinNoise self_noise;
    rt_float scale;

} Perlin;

Texture perlin_create(rt_float scale);
Vec3 perlin_get(rt_float u, rt_float v, const Vec3 *point, const Perlin *self);
