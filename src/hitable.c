#include "hitable.h"
#include <stddef.h>
#include <stdlib.h>

size_t uobj = 0;
void add_hitable_object(Object *hitable_list, Object object)
{

    HitableList *list = (HitableList *)hitable_list->data;
    add_hitable_list(list, object);
}

bool hit_call_all_object(Object *hitable_list, Ray r, rt_float t_min, rt_float t_max, HitRecord *record)
{
    return hitable_list->collide(r, t_min, t_max, record, hitable_list->data);
}
static bool hitable_list_destroy(HitableList *self)
{

    size_t i;
    for (i = 0; i < self->child_count; i++)
    {
        if (self->childs[i].data)
        {
            if (self->childs[i].destroy)
            {
                self->childs[i].destroy(self->childs[i].data);
            }
        }
    }

    return true;
}
static bool hitable_list_call_all(Ray r, rt_float t_min, rt_float t_max, HitRecord *record, const HitableList *self)
{
    HitRecord temp;
    bool at_least_one_collided = false;
    size_t i;

    rt_float closest = t_max;

    for (i = 0; i < self->child_count; i++)
    {
        if (self->childs[i].collide(r, t_min, closest, &temp, self->childs[i].data))
        {
            at_least_one_collided = true;

            closest = temp.t;
            *record = temp;
        }
    }

    return at_least_one_collided;
}

static bool hitable_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const HitableList *self)
{
    size_t i;
    AABB temp;

    bool first = true;
    if (self->child_count == 0)
    {
        return false;
    }

    for (i = 0; i < self->child_count; i++)
    {
        if (!self->childs[i].get_aabb(time_start, time_end, &temp, self->childs[i].data))
        {
            continue;
        }
        *output = first ? temp : aabb_surrounding(output, &temp);

        first = false;
    }

    return true;
}

void hit_destroy_all_objects(Object *hitable_list)
{
    hitable_list->destroy(hitable_list->data);
    free(hitable_list->data);
}

void set_face_normal(const Ray *r, const Vec3 outward, HitRecord *self)
{
    self->front_face = vec3_dot(r->direction, outward) < 0;
    self->normal = self->front_face ? outward : vec3_inv(outward);
}

Object create_hitable_list(void)
{
    Object result;
    HitableList *list = malloc(sizeof(HitableList));
    list->child_count = 0;
    result.data = list;
    result.is_leaf = false;
    result.destroy = (ObjectDestroy)hitable_list_destroy;
    result.get_aabb = (ObjectGetAABB)hitable_get_aabb;
    result.collide = (ObjectCallback)hitable_list_call_all;

    return result;
}

void hit_remove_object(HitableList *list, Object obj)
{
    size_t i;
    size_t ri = 1024;
    for (i = 0; i < list->child_count; i++)
    {
        if (list->childs[i].uid == obj.uid)
        {
            ri = i;
            break;
        }
    }
    if (ri == 1024)
    {
        return;
    }

    for (i = ri; i < list->child_count - 1; i++)
    {
        list->childs[i] = list->childs[i + 1];
    }
    list->child_count -= 1;
}

void add_hitable_list(HitableList *hitable_list, Object object)
{

    object.uid = uobj++;
    hitable_list->childs[hitable_list->child_count] = object;
    hitable_list->child_count++;
}
