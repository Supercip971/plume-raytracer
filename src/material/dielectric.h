#pragma once
#include "../hitable.h"

typedef struct
{
    rt_float ref_index;
} Dieletric;

Material dieletric_create(rt_float red_index);
bool dieletric_callback(const Ray *r_in, const HitRecord *record, Vec3 *attenuation, Ray *scattered, const Dieletric *self);
