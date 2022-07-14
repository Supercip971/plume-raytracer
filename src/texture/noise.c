
#include <stdlib.h>
#include "noise.h"
#include <texture/texture.h>

Vec3 perlin_get(rt_float u, rt_float v, const Vec3 *point, const Perlin *self)
{
    Vec3 final = vec3_mul_val(vec3_create(1, 1, 1), 0.5 * (1 + fast_sin(self->scale * point->z + 10 * turb(point, 7, &self->self_noise))));

    (void)u;
    (void)v;

    return final;
}

Texture perlin_create(rt_float scale)
{
    Perlin *perl = malloc(sizeof(Perlin));

    *perl = (Perlin){
        .scale = scale,
        .self_noise = noise_create(),
    };

    return (Texture){
        .type = TEXTURE_NOISE,
        .data = perl,
    };
}
