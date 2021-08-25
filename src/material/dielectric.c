#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "../utils.h"
#include "dielectric.h"
double schlick(double cosine, double ref_index)
{
    double r0 = (1 - ref_index) / (1 + ref_index);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow(1 - cosine, 5);
}

bool dieletric_callback(const Ray *r_in, const HitRecord *record, Vec3 *attenuation, Ray *scattered, const Dieletric *self)
{

    Vec3 direction;

    double refraction_ratio = record->front_face ? (1 / self->ref_index) : self->ref_index;
    Vec3 unit_direction = vec3_unit(r_in->direction);

    double cos_theta = fmin(vec3_dot(vec3_inv(unit_direction), record->normal), 1);
    double sin_theta = fast_sqrt(1 - cos_theta * cos_theta);

    bool cannot_refract = (refraction_ratio * sin_theta) > 1;

    if (cannot_refract || schlick(cos_theta, refraction_ratio) > random_double())
    {
        direction = reflect(unit_direction, record->normal);
    }
    else
    {
        direction = refract(unit_direction, record->normal, refraction_ratio);
    }

    scattered->direction = direction;
    scattered->origin = record->pos;

    *attenuation = vec3_create(1, 1, 1);
    return 1;
}

Material dieletric_create(double red_index)

{
    Material mat;
    Dieletric *self = malloc(sizeof(Dieletric));

    self->ref_index = red_index;

    mat.data = self;
    mat.material_callback = (MaterialCallback)dieletric_callback;
    return mat;
}
