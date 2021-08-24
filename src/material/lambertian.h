#pragma once
#include "../hitable.h"

typedef struct
{
    Vec3 albedo;
} Lambertian;
bool lambertian_callback(const Ray *r_in, const HitRecord *record, Vec3 *attenuation, Ray *scattered, const Lambertian *self);

Material lambertian_create(Vec3 albedo);
