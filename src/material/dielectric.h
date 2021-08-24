#pragma once
#include "../hitable.h"

typedef struct
{
    double ref_index;
} Dieletric;

bool dieletric_callback(const Ray *r_in, const HitRecord *record, Vec3 *attenuation, Ray *scattered, const Dieletric *self);

Material dieletric_create(double red_index);

double schlick(double cosine, double ref_index);
