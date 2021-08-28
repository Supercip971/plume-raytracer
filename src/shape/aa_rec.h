#ifndef AA_REC_H
#define AA_REC_H
#include "../aabb.h"
#include "../hitable.h"
#include "../ray.h"
#include "../vec3.h"

typedef struct aa_rec
{
    rt_float x_0;
    rt_float y_0;
    rt_float x_1;
    rt_float y_1;
    rt_float k;
    Material self_material;
    AABB self_box;

} AARect;

Object aarect_create(rt_float x_0, rt_float x_1, rt_float y_0, rt_float y_1, rt_float k, Material rect_material);

#endif
