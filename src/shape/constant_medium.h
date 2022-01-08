#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H
#include "../hitable.h"
#include "../ray.h"
#include "../vec3.h"
typedef struct
{
    rt_float d;
    Object target;
    Material phase_material;
} ConstantMedium;

Object make_constant_medium(Object obj, rt_float d, Material mat);

bool constant_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const ConstantMedium *self);

bool constant_destroy(ConstantMedium *self);

bool hit_constant_object_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const ConstantMedium *self);

#endif /* E791807C_223D_4475_BE19_6C598A188E55 */
