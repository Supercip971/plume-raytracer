#pragma once
#include "boolean.h"
#include "ray.h"

typedef struct
{
    float t;
    Vec3 pos;
    Vec3 normal;
} HitRecord;

typedef bool (*HitCallback)(Ray r, float t_min, float t_max, HitRecord *record, const void *self);

typedef struct
{
    HitCallback callback;
    bool active;
    const void *self;
} Hitable;

void add_hitable_object(HitCallback callback, const void *self);

void hit_destroy_all_objects(void);

bool hit_call_all_object(Ray r, float t_min, float t_max, HitRecord *result);
