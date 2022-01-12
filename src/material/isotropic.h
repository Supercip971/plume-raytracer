#ifndef ISOTROPIC_H
#define ISOTROPIC_H
#include "../color.h"
#include "../hitable.h"
#include "../texture/texture.h"

typedef struct
{
    Texture albedo;
} Isotropic;

Material isotropic_create(Texture albedo);
bool isotropic_callback(const Ray *r_in, const HitRecord *record, MaterialRecord *mat_record, const Isotropic *self);
#endif
