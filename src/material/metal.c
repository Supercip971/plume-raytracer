#include "metal.h"
#include <stdlib.h>

bool metal_callback(const Ray *r_in, const HitRecord *record, Vec3 *attenuation, Ray *scattered, const Metal *self)
{
    Vec3 target = reflect(vec3_unit(r_in->direction), record->normal);

    scattered->origin = record->pos;
    scattered->direction = (vec3_add(target, vec3_mul_val(random_vec3_unit(), self->fuzzy)));

    *attenuation = self->albedo;

    return vec3_dot(scattered->direction, record->normal) > 0;
}

Material metal_create(Vec3 albedo, rt_float fuzzy)
{
    Material mat;
    Metal *self = malloc(sizeof(Metal));

    self->albedo = albedo;
    self->fuzzy = fuzzy;

    mat.data = self;
    mat.material_callback = (MaterialCallback)metal_callback;
    return mat;
}
