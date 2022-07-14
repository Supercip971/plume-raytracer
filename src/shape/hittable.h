#ifndef HITTABLE_H
#define HITTABLE_H 

#include <stdbool.h>
#include <stddef.h>
#include <math/aabb.h>
#include <shape/shape.h>
#include <material/material.h>
#include <math/ray.h>
#include <shape/shape.h>

struct hit_record
{
    int depth;
    int oc_depth;
    rt_float t;
    Vec3 pos;
    Vec3 normal;
    Vec3 outward;
    rt_float u, v;
    Material material;
    bool front_face;
};

void set_face_normal(const Ray *r, const Vec3 outward, HitRecord *self);


#endif