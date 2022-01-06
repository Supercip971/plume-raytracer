#include "material.h"
#include "dielectric.h"
#include "lambertian.h"
#include "light.h"
#include "metal.h"

FLATTEN bool material_get(const Ray *r_in, const HitRecord *record, Vec3 *attenuation, Ray *scattered, const Material *self)
{
    switch (self->type)
    {
    case MATERIAL_LAMBERTIAN:
        return lambertian_callback(r_in, record, attenuation, scattered, self->data);
    case MATERIAL_METAL:
        return metal_callback(r_in, record, attenuation, scattered, self->data);
    case MATERIAL_DIELETRIC:
        return dieletric_callback(r_in, record, attenuation, scattered, self->data);
    default:
        return false;
    }
}
bool material_color_emit(rt_float u, rt_float v, const Vec3 *point, Vec3 *emit, const Material *self)
{
    switch (self->type)
    {
    case MATERIAL_LIGHT:
        return light_emit(u, v, point, emit, self->data);
    default:
        return false;
    }
}
