
#include <stdlib.h>
#include "perlin.h"

static Vec3 perlin_get(rt_float u, rt_float v, const Vec3 *point, const Perlin *self)
{

   /* Vec3 scaled_point = vec3_mul_val(*point, self->scale);*/
    Vec3 final = vec3_mul_val(vec3_create(1, 1, 1), 0.5 * (1 + sin(self->scale * point->z + 10 * turb(point, 7, &self->self_noise))));

    (void)u;
    (void)v;

    return final;
}

Texture perlin_create(rt_float scale)
{
    Texture result;
    Perlin *perl = malloc(sizeof(Perlin));

    perl->self_noise = noise_create();

    perl->scale = scale;
    result.data = perl;
    result.get_pixel = (texture_get)perlin_get;

    return result;
}
