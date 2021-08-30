#ifndef AA_REC_H
#define AA_REC_H
#include "../aabb.h"
#include "../hitable.h"
#include "../ray.h"
#include "../vec3.h"

typedef struct xy_rec
{
    rt_float x_0;
    rt_float y_0;
    rt_float x_1;
    rt_float y_1;
    rt_float k;
    Material self_material;
    AABB self_box;

} XYrec;
typedef struct xz_rec
{
    rt_float x_0;
    rt_float z_0;
    rt_float x_1;
    rt_float z_1;
    rt_float k;
    Material self_material;
    AABB self_box;

} XZrec;
typedef struct yz_rec
{
    rt_float y_0;
    rt_float z_0;
    rt_float y_1;
    rt_float z_1;
    rt_float k;
    Material self_material;
    AABB self_box;

} YZrec;

Object aaxyrect_create(rt_float x_0, rt_float x_1, rt_float y_0, rt_float y_1, rt_float k, Material rect_material);
Object aaxzrect_create(rt_float x_0, rt_float x_1, rt_float z_0, rt_float z_1, rt_float k, Material rect_material);
Object aayzrect_create(rt_float y_0, rt_float y_1, rt_float z_0, rt_float z_1, rt_float k, Material rect_material);

#endif
