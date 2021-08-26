#pragma once

#include "../vec3.h"

typedef Vec3 (*texture_get)(rt_float u, rt_float v, const Vec3 *point, const void *self);
typedef struct texture

{
    void *data;
    texture_get get_pixel;
} Texture;
