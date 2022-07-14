#ifndef TEXTURE_H
#define TEXTURE_H

#include "../vec3.h"

typedef enum
{
    TEXTURE_NONE,
    TEXTURE_CHECKER,
    TEXTURE_IMAGE,
    TEXTURE_NOISE,
    TEXTURE_SOLID_COLOR,
} TextureTypes;

typedef struct texture
{
    // this is dumb, and should be an union
    void *data;
    TextureTypes type;
} Texture;
Vec3 texture_get(rt_float u, rt_float v, const Vec3 *point, const Texture *texture);

#endif
