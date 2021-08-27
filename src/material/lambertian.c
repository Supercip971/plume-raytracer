#include "lambertian.h"
#include "../texture/solid_color.h"
#include "../utils.h"

bool lambertian_callback(const Ray *r_in, const HitRecord *record, Vec3 *attenuation, Ray *scattered, const Lambertian *self)
{
    Vec3 direction = vec3_add(record->normal, random_vec3_unit());
    (void)r_in;

    if (is_vec3_near_zero(direction))
    {
        direction = record->normal;
    }

    scattered->origin = record->pos;
    scattered->direction = direction;
    scattered->time = r_in->time;

    *attenuation = self->albedo.get_pixel(record->u, record->v, &record->pos, self->albedo.data);

    return true;
}

Material lambertian_create(Vec3 albedo)
{
    return lambertian_create_texture(solid_color_create(albedo));
}

Material lambertian_create_texture(Texture albedo)
{
    Material mat;
    Lambertian *self = malloc(sizeof(Lambertian));

    self->albedo = albedo;

    mat.data = self;
    mat.material_callback = (MaterialCallback)lambertian_callback;

    return mat;
}
