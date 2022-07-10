#pragma once

/* todo: implement a better data structure */

#include "hitable.h"
typedef struct bvh_data
{
    Object left;
    Object right;
    AABB box;
    bool has_conflict;
} bvhData;

bool bvh_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const bvhData *self);
bool bvh_destroy(bvhData *self);
bool bvh_hit(Ray r, rt_float t_min, rt_float t_max, HitRecord *record, const bvhData *self);

void bvh_create_rec(HitableList *list, rt_float tstart, rt_float tend);

rt_float bvh_pdf_value(Vec3 origin, Vec3 direction, const bvhData *self);
Vec3 bvh_random(Vec3 origin, const bvhData *self);
