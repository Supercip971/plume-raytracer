#ifndef HITABLE_H
#define HITABLE_H
#include <stdbool.h>
#include <stddef.h>
#include "aabb.h"
#include "ray.h"

typedef struct hit_record HitRecord;
typedef struct object Object;

typedef bool (*MaterialCallback)(const Ray *r_in, const HitRecord *record, Vec3 *attenuation, Ray *scattered, const void *self);
typedef bool (*MaterialColorEmit)(rt_float u, rt_float v, const Vec3 *point, Vec3 *emit, const void *self);

typedef bool (*ObjectCallback)(Ray r, rt_float t_min, rt_float t_max, HitRecord *record, const void *self);

typedef bool (*ObjectGetAABB)(rt_float time_start, rt_float time_end, AABB *output, const void *self);
typedef bool (*ObjectDestroy)(void *self);

typedef struct material
{
    MaterialCallback material_callback;
    MaterialColorEmit color_emition;
    void *data;
} Material;

struct object
{
    ObjectCallback collide;
    ObjectGetAABB get_aabb;
    ObjectDestroy destroy;
    void *data;
    bool is_leaf;
};

struct hit_record
{
    rt_float t;
    Vec3 pos;
    Vec3 normal;
    rt_float u, v;
    Material material;
    bool front_face;
};

typedef struct hitable_list
{
    Object childs[512]; /* todo dynamically alloc this */
    size_t child_count;
} HitableList;

void add_hitable_object(Object *hitable_list, Object object);

void hit_destroy_all_objects(Object *hitable_list);

bool hit_call_all_object(Object *hitable_list, Ray r, rt_float t_min, rt_float t_max, HitRecord *record);

void set_face_normal(const Ray *r, const Vec3 outward, HitRecord *self);

Object create_hitable_list(void);
#endif
