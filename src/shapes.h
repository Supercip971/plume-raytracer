#pragma once
#include "boolean.h"
#include "hitable.h"
#include "material.h"
#include "ray.h"
#include "vec3.h"
typedef struct sphere
{
    double radius;
    Vec3 pos;
} Sphere;

bool hit_sphere_object_callback(Ray ray, double t_min, double t_max, HitRecord *record, const Sphere *self) ;

Sphere *sphere_create(double radius, Vec3 pos);
