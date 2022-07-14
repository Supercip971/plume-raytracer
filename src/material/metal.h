#pragma once
#include <material/material.h>

typedef struct
{
    Vec3 albedo;
    rt_float fuzzy;
} Metal;

bool metal_callback(const Ray *r_in, const HitRecord *record, MaterialRecord *mat_record, const Metal *self);

Material metal_create(Vec3 albedo, rt_float fuzzy);
