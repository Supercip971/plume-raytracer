#include "lambertian.h"
#include "utils.h"

bool lambertian_callback(const Ray *r_in, const HitRecord *record, Vec3 *attenuation, Ray *scattered, const Lambertian *self)
{
    Vec3 target = vec3_add(record->pos, vec3_add(record->normal, vec3_create(random_double(), random_double(), random_double())));
    scattered->origin = target;
    scattered->direction = vec3_sub(target, record->pos);
    *attenuation = self->albedo;
    (void)r_in;
    return true;
}

Material lambertian_create(Vec3 albedo)
{
    Lambertian *self = malloc(sizeof(Lambertian));
    Material mat;
    self->albedo = albedo;

    mat.data = self;
    mat.material_callback = (MaterialCallback)lambertian_callback;
    return mat;
}
