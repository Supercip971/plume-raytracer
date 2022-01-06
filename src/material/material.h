#ifndef __MATERIAL_H__
#define __MATERIAL_H__
#include "../aabb.h"
#include "../ray.h"
typedef enum
{
    MATERIAL_NONE,
    MATERIAL_DIELETRIC,
    MATERIAL_LAMBERTIAN,
    MATERIAL_LIGHT,
    MATERIAL_METAL,
} MaterialTypes;

typedef struct material
{
    MaterialTypes type;
    void *data;
} Material;

typedef struct hit_record HitRecord;

bool material_get(const Ray *r_in, const HitRecord *record, Vec3 *attenuation, Ray *scattered, const Material *self);
bool material_color_emit(rt_float u, rt_float v, const Vec3 *point, Vec3 *emit, const Material *self);

#endif /* */
