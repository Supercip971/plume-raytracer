#pragma once
#include "../perlin.h"
#include "texture.h"

typedef struct perlin
{
    PerlinNoise self_noise;
    rt_float scale;

} Perlin;

Texture perlin_create(rt_float scale);
