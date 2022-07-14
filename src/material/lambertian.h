#pragma once
#include <material/material.h>
#include <texture/texture.h>

typedef struct
{
    Texture albedo;
} Lambertian;

bool lambertian_callback(const Ray *r_in, const HitRecord *record, MaterialRecord *mat_record, const Lambertian *self);
rt_float lambertian_get_pdf(const Ray *r_in, const HitRecord *record, const Ray *scattered, const Lambertian *self);

Material lambertian_create(Vec3 albedo);
Material lambertian_create_texture(Texture albedo);
