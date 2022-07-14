#pragma once
#include <hitable.h>

typedef struct
{
    rt_float ref_index;
} Dieletric;

Material dieletric_create(rt_float red_index);
bool dieletric_callback(const Ray *r_in, const HitRecord *record, MaterialRecord *mat_record,
                        const Dieletric *self);
