#include "light.h"
#include <stdlib.h>
#include "../texture/solid_color.h"

bool light_emit(rt_float u, rt_float v, const Vec3 *point, Vec3 *emit, const Light *self)
{
    *emit = texture_get(u, v, point, &self->emition);

    return true;
}
Material light_create_texture(Texture emition)
{
    Material mat;
    Light *light = malloc(sizeof(Light));

    light->emition = emition;
    mat.data = light;
    mat.type = MATERIAL_LIGHT;
    return mat;
}

Material light_create(Vec3 emition)
{
    return light_create_texture(solid_color_create(emition));
}
