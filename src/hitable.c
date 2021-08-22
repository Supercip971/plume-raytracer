#include "hitable.h"
#include <stddef.h>
#include <stdlib.h>

size_t hitable_count = 0;
Hitable hitable_table[512];
void add_hitable_object(HitCallback callback, const void *self)
{
    Hitable res;

    res.active = true;
    res.callback = callback;
    res.self = self;

    hitable_table[hitable_count] = res;
    hitable_count++;
}

bool hit_call_all_object(Ray r, float t_min, float t_max, HitRecord *result)
{
    HitRecord temp;
    bool hit_anything = false;
    float closest = t_max;
    size_t i;
    for (i = 0; i < hitable_count; i++)
    {
        if (hitable_table[i].callback(r, t_min, closest, &temp, hitable_table[i].self))
        {
            hit_anything = true;
            closest = temp.t;
            *result = temp;
        }
    }
    return hit_anything;
}

void hit_destroy_all_objects(void)
{
    size_t i;
    for (i = 0; i < hitable_count; i++)
    {
        if (hitable_table[i].self)
        {
            free((void *)hitable_table[i].self);
        }
    }
}
