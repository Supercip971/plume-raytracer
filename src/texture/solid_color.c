
#include "solid_color.h"
#include <stdlib.h>

static Vec3 solid_color_get(rt_float u, rt_float v, const Vec3 *point, const SolidColor *self)
{
    (void)u;
    (void)v;
    (void)point;
    return self->color;
}

Texture solid_color_create(Vec3 color)
{
    Texture result;
    SolidColor *col = malloc(sizeof(SolidColor));

    col->color = color;

    result.data = col;
    result.get_pixel = (texture_get)solid_color_get;

    return result;
}
