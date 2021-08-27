#pragma once
#include "texture.h"

typedef struct solid_color
{
    Vec3 color;
} SolidColor;

Texture solid_color_create(Vec3 color);
