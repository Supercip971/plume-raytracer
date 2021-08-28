#pragma once
#include "../hitable.h"
#include "../texture/texture.h"
#include "../utils.h"
#include "../vec3.h"

typedef struct
{
    Texture emition;
} Light;

Material light_create(Vec3 emition);
Material light_create_texture(Texture emition);
