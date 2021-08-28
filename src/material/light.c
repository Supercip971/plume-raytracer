#include "light.h"
#include <stdlib.h>
#include "../texture/solid_color.h"

static bool light_emit(rt_float u, rt_float v, Vec3 *point, Vec3 *emit, const Light *self)
{

    *emit = self->emition.get_pixel(u, v, point, self->emition.data);

    return true;
}
static bool light_callback(const Ray *r_in, const HitRecord *record, Vec3 *attenuation, Ray *scattered, const Light *self)
{
    (void)r_in;
    (void)record;
    (void)attenuation;
    (void)scattered;
    (void)self;
    return false;
}

Material light_create_texture(Texture emition)
{
    Material mat;
    Light *light = malloc(sizeof(Light));

    light->emition = emition;
    mat.data = light;
    mat.color_emition = (MaterialColorEmit)light_emit;
    mat.material_callback = (MaterialCallback)light_callback;

    return mat;
}

Material light_create(Vec3 emition)
{
    return light_create_texture(solid_color_create(emition));
}
