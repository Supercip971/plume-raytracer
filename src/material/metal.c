#include "metal.h"
#include <stdlib.h>
#include "material/material.h"

bool metal_callback(const Ray *r_in, const HitRecord *record, MaterialRecord *mat_record, const Metal *self)
{

    Vec3 target = reflect(vec3_unit(r_in->direction), record->normal);

    mat_record->scattered.origin = record->pos;
    mat_record->scattered.direction = (vec3_add(target, vec3_mul_val(random_vec3_unit(), self->fuzzy)));
    mat_record->scattered.time = r_in->time;
    mat_record->attenuation = self->albedo;
    mat_record->is_specular = true;
    mat_record->pdf.type = PDF_NONE;

    return vec3_dot(mat_record->scattered.direction, record->normal) > 0;
}

Material metal_create(Vec3 albedo, rt_float fuzzy)
{
    Metal *self = malloc(sizeof(Metal));

    *self = (Metal){
        .albedo = albedo,
        .fuzzy = fuzzy,
    };

    return (Material){
        .data = self,
        .type = MATERIAL_METAL,
    };
}
