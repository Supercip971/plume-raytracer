#pragma once
#include "../hitable.h"

typedef struct
{
    Vec3 albedo;
    double fuzzy;
} Metal;

bool metal_callback(const Ray *r_in, const HitRecord *record, Vec3 *attenuation, Ray *scattered, const Metal *self);

Material metal_create(Vec3 albedo, double fuzzy);
