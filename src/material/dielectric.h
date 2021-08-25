#pragma once
#include "../hitable.h"

typedef struct
{
    rt_float ref_index;
} Dieletric;

bool dieletric_callback(const Ray *r_in, const HitRecord *record, Vec3 *attenuation, Ray *scattered, const Dieletric *self);

Material dieletric_create(rt_float red_index);

rt_float schlick(rt_float cosine, rt_float ref_index);
