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
    /*
    Vec3 outward_normal;
    Vec3 reflected = reflect(r_in->direction, record->normal);
    Vec3 refracted = {0};
    double ni_over_nt;
    double reflect_prob;
    double cosine;
    *attenuation = vec3_create(1, 1, 1);

    

    if (vec3_dot(r_in->direction, record->normal) > 0)
    {
        outward_normal = vec3_inv(record->normal);
        ni_over_nt = self->ref_index;
        cosine = self->ref_index * (vec3_dot(r_in->direction, record->normal) / vec3_length(r_in->direction));
    }
    else
    {
        outward_normal = (record->normal);
        ni_over_nt = 1 / self->ref_index;
        cosine = -(vec3_dot(r_in->direction, record->normal) / vec3_length(r_in->direction));
    }

    scattered->origin = record->pos;
    if (refract(&refracted, r_in->direction, outward_normal, ni_over_nt))
    {
        reflect_prob = schlick(cosine, self->ref_index);
    }
    else
    {
        reflect_prob = 1;
    }
    if (random_double() < reflect_prob)
    {

        scattered->direction = reflected;
    }

    else
    {
        scattered->direction = refracted;
    }
*/
    double refraction_ratio = record->front_face ? (1 / self->ref_index) : self->ref_index;
    Vec3 unit_direction = vec3_unit(r_in->direction);
    double cos_theta = fmin(vec3_dot(vec3_inv(unit_direction), record->normal), 1);
    double sin_theta = fast_sqrt(1 - cos_theta * cos_theta);

    bool cannot_refract = (refraction_ratio * sin_theta) > 1;
    Vec3 direction;

    if (cannot_refract || schlick(cos_theta, refraction_ratio) > random_double())
    {
        direction = reflect(unit_direction, record->normal);
    }
    else
    {
        refract(&direction, unit_direction, record->normal, refraction_ratio);
    }
    scattered->direction = direction;
    scattered->origin = record->pos;

    *attenuation = vec3_create(1, 1, 1);
    return 1;
}

Material dieletric_create(double red_index)

{
    Dieletric *self = malloc(sizeof(Dieletric));
    Material mat;
    self->ref_index = red_index;

    mat.data = self;
    mat.material_callback = (MaterialCallback)dieletric_callback;
    return mat;
}
