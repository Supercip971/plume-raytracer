#include "hitable.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <shape/shape.h>

size_t uobj = 0;

void add_hitable_object(Object *hitable_list, Object object)
{
    HitableList *list = (HitableList *)hitable_list->data;
    add_hitable_list(list, object);
}

bool hit_call_all_object(Object *hitable_list, Ray r, rt_float t_min, rt_float t_max, HitRecord *record)
{
    return object_collide(r, t_min, t_max, record, hitable_list);
}

bool hitable_list_destroy(HitableList *self)
{
    for (size_t i = 0; i < self->child_count; i++)
    {
        object_destroy(&self->childs[i]);
    }

    free(self->childs);
    return true;
}

bool hitable_list_call_all(Ray r, rt_float t_min, rt_float t_max, HitRecord *record, const HitableList *self)
{
    bool at_least_one_collided = false;

    rt_float closest = t_max;

    for (size_t i = 0; i < self->child_count; i++)
    {
        AABB box;
        object_get_aabb(0, 1000, &box, &self->childs[i]);

        if (aabb_hit(&box, &r, t_min, closest))
        {

            HitRecord temp;
            if (object_collide(r, t_min, closest, &temp, (Object *)&self->childs[i]))
            {
                at_least_one_collided = true;

                closest = temp.t;
                *record = temp;
            }
        }
    }

    return at_least_one_collided;
}

bool hit_call_all_list(const HitableList *hitable_list, Ray r, rt_float t_min, rt_float t_max, HitRecord *record)
{
    return hitable_list_call_all(r, t_min, t_max, record, hitable_list);
}

static bool hitable_get_all_aabb(rt_float time_start, rt_float time_end, AABB *output, const HitableList *self)
{
    AABB temp;
    bool first = true;

    for (size_t i = 0; i < self->child_count; i++)
    {
        if (!object_get_aabb(time_start, time_end, &temp, (Object *)&self->childs[i]))
        {
            continue;
        }
        *output = first ? temp : aabb_surrounding(output, &temp);

        first = false;
    }

    return true;
}
bool hitable_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const HitableList *self)
{
    (void)time_start;
    (void)time_end;
    *output = self->bounding_box;
    return true;
}

void hit_destroy_all_objects(Object *hitable_list)
{
    object_destroy(hitable_list);
}

void set_face_normal(const Ray *r, const Vec3 outward, HitRecord *self)
{

    self->outward = outward;
    self->front_face = vec3_dot(r->direction, outward) < 0;
    self->normal = self->front_face ? outward : vec3_neg(outward);
}

Object create_hitable_list(void)
{
    HitableList *list = malloc(sizeof(HitableList));
    *list = (HitableList){
        .child_count = 0,
        .childs = malloc(sizeof(Object) * 64),
        .allocated_childs = 64,
    };

    Object result = {
        .data = list,
        .is_leaf = false,
        .type = SHAPE_HITABLE_LIST,
    };

    return result;
}

void hit_remove_object(HitableList *list, Object obj)
{
    size_t ri = (size_t)-1;
    for (size_t i = 0; i < list->child_count; i++)
    {
        if (list->childs[i].uid == obj.uid)
        {
            ri = i;
            break;
        }
    }

    if (ri == (size_t)-1)
    {
        return;
    }

    for (size_t i = ri; i < list->child_count - 1; i++)
    {
        list->childs[i] = list->childs[i + 1];
    }
    list->child_count -= 1;

    hitable_get_all_aabb(0, 10000, &list->bounding_box, list);
}

void add_hitable_list(HitableList *hitable_list, Object object)
{
    object.uid = uobj++;
    hitable_list->childs[hitable_list->child_count] = object;
    hitable_list->child_count++;

    if (hitable_list->child_count == hitable_list->allocated_childs)
    {
        hitable_list->allocated_childs *= 2;
        hitable_list->childs = realloc(hitable_list->childs, hitable_list->allocated_childs * sizeof(Object));
    }

    hitable_get_all_aabb(0, 100000, &hitable_list->bounding_box, hitable_list);
}

Vec3 hitable_random(Vec3 origin, const HitableList *self)
{
    if (self->child_count == 0)
    {
        return vec3_create(0, 0, 1);
    }

    return object_random(origin, &self->childs[(int)rand() % self->child_count]);
}

rt_float hitable_pdf_value(Vec3 origin, Vec3 direction, const HitableList *self)
{
    rt_float res = 0;
    rt_float weight = 1.0 / self->child_count;

    for (size_t i = 0; i < self->child_count; i++)
    {
        res += object_pdf_value(origin, direction, &self->childs[i]) * weight;
    }

    return res;
}
