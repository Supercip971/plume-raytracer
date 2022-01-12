#include "metal.h"
#include <stdlib.h>

bool metal_callback(const Ray *r_in, const HitRecord *record, MaterialRecord *mat_record, const Metal *self)
{

    Vec3 target = reflect(vec3_unit(r_in->direction), record->normal);

    mat_record->scattered.origin = record->pos;
    mat_record->scattered.direction = (vec3_add(target, vec3_mul_val(random_vec3_unit(), self->fuzzy)));
    mat_record->scattered.time = r_in->time;
    mat_record->attenuation = self->albedo;
    mat_record->is_specular = false;
    mat_record->pdf.type = PDF_NONE;

    return vec3_dot(mat_record->scattered.direction, record->normal) > 0;
}

Material metal_create(Vec3 albedo, rt_float fuzzy)
{
    Material mat;
    Metal *self = malloc(sizeof(Metal));

    self->albedo = albedo;
    self->fuzzy = fuzzy;

    mat.data = self;
    mat.type = MATERIAL_METAL;
    return mat;
}
