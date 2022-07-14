#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils/utils.h>
#include "dielectric.h"
#include <shape/hittable.h>


static rt_float schlick(rt_float cosine, rt_float ref_index)
{
    rt_float r0 = (1.0 - ref_index) / (1.0 + ref_index);
    r0 = r0 * r0;
    rt_float v = 1 - cosine;
    return r0 + (1.0 - r0) * powf(v, 5);
}

bool dieletric_callback(const Ray *r_in, const HitRecord *record, MaterialRecord *mat_record,
                        const Dieletric *self)
{

    rt_float refraction_ratio = record->front_face ? (1.0f / self->ref_index) : self->ref_index;
    Vec3 unit_direction = vec3_unit(r_in->direction);

    rt_float cos_theta = rt_min(vec3_dot(vec3_neg(unit_direction), record->normal), 1);
    rt_float sin_theta = fast_sqrt(1 - cos_theta * cos_theta);

    bool cannot_refract = (refraction_ratio * sin_theta) > 1;

    Vec3 direction;
    if (cannot_refract || schlick(cos_theta, refraction_ratio) > random_rt_float())
    {
        direction = reflect(unit_direction, record->normal);
    }
    else
    {
        direction = refract(unit_direction, record->normal, refraction_ratio);
    }

    mat_record->pdf.type = PDF_NONE;
    mat_record->scattered.direction = direction;
    mat_record->scattered.origin = record->pos;
    mat_record->scattered.time = r_in->time;

    mat_record->is_specular = true;
    mat_record->attenuation = vec3_create(1, 1, 1);
    return 1;
}

Material dieletric_create(rt_float red_index)
{
    Material mat;
    Dieletric *self = malloc(sizeof(Dieletric));

    self->ref_index = red_index;

    mat.data = self;
    mat.type = MATERIAL_DIELETRIC;
    return mat;
}
