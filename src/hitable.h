#pragma once
#include "boolean.h"
#include "material.h"
#include "ray.h"

typedef struct hit_record HitRecord;

typedef bool (*MaterialCallback)(const Ray *r_in, const HitRecord *record, Vec3 *attenuation, Ray *scattered, const void *self);

typedef bool (*HitCallback)(Ray r, double t_min, double t_max, HitRecord *record, const void *self);

typedef struct material
{
    MaterialCallback material_callback;
    void *data;
} Material;

struct hit_record
{
    double t;
    Vec3 pos;
    Vec3 normal;
    Material material;
};

typedef struct
{
    HitCallback callback;
    Material material;
    bool active;
    const void *object;
} Hitable;

void add_hitable_object(HitCallback callback, const void *object, Material material);

void hit_destroy_all_objects(void);

bool hit_call_all_object(Ray r, double t_min, double t_max, HitRecord *result);
