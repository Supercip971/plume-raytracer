#pragma once
#include "texture.h"

typedef struct checker_texture
{
    Texture even_color;
    Texture odd_color;
    rt_float size;
} CheckerTexture;

Texture checker_create(Texture even, Texture odd, rt_float size);

Texture checker_create_col(Vec3 even, Vec3 odd, rt_float size);
Vec3 checker_get(rt_float u, rt_float v, const Vec3 *point, const CheckerTexture *self);
