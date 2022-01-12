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

rt_float aa_xyrect_pdf_value(Vec3 origin, Vec3 direction, XYrec *self);
rt_float aa_xzrect_pdf_value(Vec3 origin, Vec3 direction, XZrec *self);
rt_float aa_yzrect_pdf_value(Vec3 origin, Vec3 direction, YZrec *self);

Vec3 aa_xyrect_pdf_random(Vec3 origin, XYrec *self);
Vec3 aa_xzrect_pdf_random(Vec3 origin, XZrec *self);
Vec3 aa_yzrect_pdf_random(Vec3 origin, YZrec *self);

bool hit_aaxyrect_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const XYrec *self);
bool hit_aaxzrect_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const XZrec *self);
bool hit_aayzrect_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const YZrec *self);

bool aaxyrect_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const XYrec *self);
bool aaxzrect_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const XZrec *self);
bool aayzrect_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const YZrec *self);

Object aaxyrect_create(rt_float x_0, rt_float x_1, rt_float y_0, rt_float y_1, rt_float k, Material rect_material);
Object aaxzrect_create(rt_float x_0, rt_float x_1, rt_float z_0, rt_float z_1, rt_float k, Material rect_material);
Object aayzrect_create(rt_float y_0, rt_float y_1, rt_float z_0, rt_float z_1, rt_float k, Material rect_material);

#endif
