#ifndef HITABLE_H
#define HITABLE_H
#include <material/material.h>
#include <math/aabb.h>
#include <math/ray.h>
#include <shape/shape.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct hitable_list
{
    Object *childs; /* todo dynamically alloc this */
    size_t allocated_childs;
    size_t child_count;
    AABB bounding_box;
} HitableList;

void add_hitable_object(Object *hitable_list, Object object);

void hit_destroy_all_objects(Object *hitable_list);

bool hit_call_all_object(Object *hitable_list, Ray r, rt_float t_min, rt_float t_max, HitRecord *record);

Object create_hitable_list(void);

void add_hitable_list(HitableList *hitable_list, Object object);
bool hit_call_all_list(const HitableList *hitable_list, Ray r, rt_float t_min, rt_float t_max, HitRecord *record);

bool hitable_list_call_all(Ray r, rt_float t_min, rt_float t_max, HitRecord *record, const HitableList *self);
bool hitable_list_destroy(HitableList *self);
bool hitable_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const HitableList *self);

Vec3 hitable_random(Vec3 origin, const HitableList *self);
rt_float hitable_pdf_value(Vec3 origin, Vec3 direction, const HitableList *self);

#endif
