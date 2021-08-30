#ifndef BOX_H
#define BOX_H
#include "../hitable.h"
#include "../ray.h"
#include "../vec3.h"

typedef struct box
{
    AABB self_bounding_box; /* put here as we need to calculate it once */
    HitableList list;
} Box;

Object box_create(Vec3 posa, Vec3 posb, Material box_material);
#endif
