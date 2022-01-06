#include "texture.h"
#include "checker.h"
#include "image.h"
#include "noise.h"
#include "solid_color.h"
Vec3 texture_get(rt_float u, rt_float v, const Vec3 *point, const Texture *texture)
{
    switch (texture->type)
    {
    case TEXTURE_NONE:
        return vec3_create(0, 0, 0);
    case TEXTURE_CHECKER:
        return checker_get(u, v, point, texture->data);
    case TEXTURE_IMAGE:
        return image_tex_get(u, v, point, texture->data);
    case TEXTURE_NOISE:
        return perlin_get(u, v, point, texture->data);
    case TEXTURE_SOLID_COLOR:
        return solid_color_get(u, v, point, texture->data);
    default:
        return vec3_create(0, 0, 0);
    }
}
