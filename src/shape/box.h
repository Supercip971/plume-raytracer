#ifndef BOX_H
#define BOX_H
#include "../hitable.h"
#include "../ray.h"
#include "../vec3.h"

typedef struct box
{
    AABB self_bounding_box; /* put here as we need to calculate it once */

    Object list;
} Box;
bool box_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const Box *self);

bool box_destroy(Box *self);

bool hit_box_object_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const Box *self);

Object box_create(Vec3 posa, Vec3 posb, Material box_material);
#endif
