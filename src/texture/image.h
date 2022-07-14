#pragma once
#include <gfx/image.h>
#include <texture/texture.h>

typedef struct image_texture
{
    Image self_picture;
    int bytes_per_scan_lines;
} ImageTexture;

Texture image_create(Image image);
Vec3 image_tex_get(rt_float u, rt_float v, const Vec3 *point, const ImageTexture *self);
