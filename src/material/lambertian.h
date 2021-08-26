#pragma once
#include "../hitable.h"
#include "../texture/texture.h"

typedef struct
{
    Texture albedo;
} Lambertian;
bool lambertian_callback(const Ray *r_in, const HitRecord *record, Vec3 *attenuation, Ray *scattered, const Lambertian *self);

Material lambertian_create(Vec3 albedo);
Material lambertian_create_texture(Texture albedo);
