#include "hitable.h"
#include <stddef.h>
#include <stdlib.h>

size_t hitable_count = 0;
Hitable hitable_table[512];
void add_hitable_object(HitCallback callback, const void *self, Material mat)
{
    Hitable res;

    res.active = true;
    res.callback = callback;
    res.object = self;
    res.material = mat;

    hitable_table[hitable_count] = res;
    hitable_count++;
}

bool hit_call_all_object(Ray r, double t_min, double t_max, HitRecord *result)
{
    HitRecord temp;
    size_t i;

    bool hit_anything = false;
    double closest = t_max;

    for (i = 0; i < hitable_count; i++)
    {
        if (hitable_table[i].callback(r, t_min, closest, &temp, hitable_table[i].object))
        {
            hit_anything = true;

            closest = temp.t;
            *result = temp;

            result->material = hitable_table[i].material;
        }
    }
    
    return hit_anything;
}

void hit_destroy_all_objects(void)
{
    size_t i;
    for (i = 0; i < hitable_count; i++)
    {
        if (hitable_table[i].object)
        {
            free((void *)hitable_table[i].object);
        }

        if (hitable_table[i].material.data)
        {
            free((void *)hitable_table[i].material.data);
        }
    }
}

void set_face_normal(const Ray *r, const Vec3 outward, HitRecord *self)
{
    self->front_face = vec3_dot(r->direction, outward) < 0;
    self->normal = self->front_face ? outward : vec3_inv(outward);
}
