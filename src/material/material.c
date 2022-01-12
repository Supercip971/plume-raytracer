#include "material.h"
#include "dielectric.h"
#include "isotropic.h"
#include "lambertian.h"
#include "light.h"
#include "metal.h"

FLATTEN bool material_get(const Ray *r_in, const HitRecord *record, MaterialRecord *mat_record, const Material *self)
{
    switch (self->type)
    {
    case MATERIAL_LAMBERTIAN:
        return lambertian_callback(r_in, record, mat_record, self->data);
    case MATERIAL_METAL:
        return metal_callback(r_in, record, mat_record, self->data);
    case MATERIAL_DIELETRIC:
        return dieletric_callback(r_in, record, mat_record, self->data);
    case MATERIAL_ISOTROPIC:
        return isotropic_callback(r_in, record, mat_record, self->data);
    default:
        return false;
    }
}
bool material_color_emit(const HitRecord *record, Vec3 *emit, const Material *self)
{
    switch (self->type)
    {
    case MATERIAL_LIGHT:
        return light_emit(record, emit, self->data);
    default:
        return false;
    }
}
rt_float material_get_pdf(const Ray *r_in, const HitRecord *record, const Ray *scattered, const Material *self)
{
    switch (self->type)
    {
    case MATERIAL_LAMBERTIAN:
        return lambertian_get_pdf(r_in, record, scattered, self->data);
    default:
        return 0;
    }
}
