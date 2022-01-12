#ifndef __MATERIAL_H__
#define __MATERIAL_H__
#include "../aabb.h"
#include "../pdf/pdf.h"
#include "../ray.h"

typedef enum
{
    MATERIAL_NONE,
    MATERIAL_DIELETRIC,
    MATERIAL_LAMBERTIAN,
    MATERIAL_LIGHT,
    MATERIAL_METAL,
    MATERIAL_ISOTROPIC,
} MaterialTypes;

typedef struct material
{
    MaterialTypes type;
    void *data;
} Material;

typedef struct
{
    Ray scattered;
    bool is_specular;
    Vec3 attenuation;
    Pdf pdf;
} MaterialRecord;

typedef struct hit_record HitRecord;

rt_float material_get_pdf(const Ray *r_in, const HitRecord *record, const Ray *scattered, const Material *self);
bool material_get(const Ray *r_in, const HitRecord *record, MaterialRecord *mat_record, const Material *self);
bool material_color_emit(const HitRecord *record, Vec3 *emit, const Material *self);

#endif /* */
