
#include <stdlib.h>
#include "checker.h"
#include "solid_color.h"
#include "texture/texture.h"

Vec3 checker_get(rt_float u, rt_float v, const Vec3 *point, const CheckerTexture *self)
{
    rt_float sines = fast_sin(self->size * point->x) * fast_sin(self->size * point->y) * fast_sin(self->size * point->z);
    if (sines < 0)
    {
        return texture_get(u, v, point, &self->odd_color);
    }
    else
    {
        return texture_get(u, v, point, &self->even_color);
    }
}

Texture checker_create(Texture even, Texture odd, rt_float size)
{
    CheckerTexture *col = malloc(sizeof(CheckerTexture));

    *col = (CheckerTexture){
        .odd_color = odd,
        .even_color = even,
        .size = size,
    };

    return (Texture){
        .type = TEXTURE_CHECKER,
        .data = col,
    };
}

Texture checker_create_col(Vec3 even, Vec3 odd, rt_float size)
{
    return checker_create(solid_color_create(even), solid_color_create(odd), size);
}
