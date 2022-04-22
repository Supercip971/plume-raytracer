#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#include "../utils.h"

Vec3 image_tex_get(rt_float u, rt_float v, const Vec3 *point, const ImageTexture *self)
{

    int i;
    int j;
    Color *pixel;

    if (self->self_picture.converted_data == NULL)
    {
        return vec3_create(0, 1, 1);
    }

    u = rt_clamp(u, 0, 1);
    v = 1-rt_clamp(v, 0, 1); /* use flip coord */

    i = (int)(u * self->self_picture.width);
    j = (int)(v * self->self_picture.height);

    i = rt_min(i, self->self_picture.width - 1);
    j = rt_min(j, self->self_picture.height - 1);

    pixel = self->self_picture.converted_data + (j * self->self_picture.width + i);
    (void)point;


    return vec_from_color(*pixel);
}

Texture image_create(Image image)
{
    Texture result;
    ImageTexture *tex = malloc(sizeof(ImageTexture));

    tex->self_picture = image;
    tex->bytes_per_scan_lines = image.width;

    result.data = tex;
    result.type = TEXTURE_IMAGE;

    return result;
}
