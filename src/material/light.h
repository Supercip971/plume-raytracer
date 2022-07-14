#pragma once
#include <material/material.h>

#include <texture/texture.h>
#include <utils/utils.h>
#include <math/vec3.h>

typedef struct
{
    Texture emition;
    bool flipped;
} Light;

Material light_create(Vec3 emition);
Material light_create_texture(Texture emition);
bool light_emit(const HitRecord *record, Vec3 *emit, const Light *self);
