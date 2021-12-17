#pragma once 
#include "aabb.h"
#include "hitable.h"
typedef struct octree 
{
    struct octree** childs;
    Object** objects;
    uint16_t obj_count;

    AABB self_box;
} Octree ;

Octree* octree_create(HitableList* hitable, rt_float t_end, rt_float t_start);

bool octree_hit(Ray r, rt_float t_min, rt_float* t_max, HitRecord *record, const Octree *self);
