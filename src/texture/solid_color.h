#pragma once
#include "texture.h"

typedef struct solid_color
{
    Vec3 color;
} SolidColor;

Texture solid_color_create(Vec3 color);
Vec3 solid_color_get(rt_float u, rt_float v, const Vec3 *point, const SolidColor *self);
