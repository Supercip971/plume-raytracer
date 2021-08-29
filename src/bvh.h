#pragma once

/* todo: implement a better data structure */

#include "hitable.h"
typedef struct bvh_data
{
    Object left;
    Object right;
    AABB box;
} bvhData;

void bvh_create_rec(HitableList *list, rt_float tstart, rt_float tend);
