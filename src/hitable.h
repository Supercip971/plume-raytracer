#pragma once
#include "boolean.h"
#include "material.h"
#include "ray.h"

typedef struct hit_record HitRecord;

typedef bool (*MaterialCallback)(const Ray *r_in, const HitRecord *record, Vec3 *attenuation, Ray *scattered, const void *self);

typedef bool (*ObjectCallback)(Ray r, rt_float t_min, rt_float t_max, HitRecord *record, const void *self);

typedef struct material
{
    MaterialCallback material_callback;
    void *data;
} Material;

typedef struct object
{
    ObjectCallback collide;
    void *data;
} Object;

struct hit_record
{
    rt_float t;
    Vec3 pos;
    Vec3 normal;
    Material material;
    bool front_face;
};

typedef struct
{
    Object object;
    Material material;
    bool active;
} Hitable;

void add_hitable_object(Object object, Material material);

void hit_destroy_all_objects(void);

bool hit_call_all_object(Ray r, rt_float t_min, rt_float t_max, HitRecord *result);

void set_face_normal(const Ray *r, const Vec3 outward, HitRecord *self);
