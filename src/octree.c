#include "octree.h"
#include <math.h>
#include <stdio.h>
static AABB get_hitable_surrounding(HitableList *hitable, rt_float t_end, rt_float t_start)
{
    AABB current;
    size_t i;
    for (i = 0; i < hitable->child_count; i++)
    {
        AABB temp;
        if (hitable->childs[i].get_aabb(t_start, t_end, &temp, hitable->childs[i].data) == false)
        {
            exit(1);
        }
        if (i == 0)
        {
            current = temp;
        }
        else
        {
            current = aabb_surrounding(&current, &temp);
        }
    }
    return current;
}

static bool octree_setup(Octree *target, AABB box, HitableList *hitable, int subdivide)
{
    size_t object_count = 0;
    int v[512];
    size_t x, y, z, i;

    int box_id = 0;

    Vec3 box_size = vec3_sub(box.max, box.min);
    Vec3 box_center = vec3_add(box.min, vec3_mul_val(box_size, 0.5f));

    target->self_box = box;
    for (i = 0; i < hitable->child_count; i++)
    {
        AABB temp;
        hitable->childs[i].get_aabb(0, 1, &temp, hitable->childs[i].data);

        if (aabb_intersect(&temp, &box))
        {
            v[object_count] = i;
            object_count++;
        }
    }

    if ((object_count == 1) || subdivide > 8)

    {
        target->obj_count = object_count;
        target->objects = malloc(sizeof(Object *) * object_count);

        for (i = 0; i < object_count; i++)
        {
            target->objects[i] = &hitable->childs[v[i]];
        }

        return true;
    }
    else if (object_count == 0)
    {
        target->obj_count = 0;
        return false;
    }
    target->obj_count = 0;
    target->childs = malloc(sizeof(Octree *) * 9);
    if (target->childs == NULL)
    {
        printf("failed to alloc ! \n");
    }
    bool at_least_one = false;
    for (x = 0; x < 2; x++)
    {
        for (y = 0; y < 2; y++)
        {
            for (z = 0; z < 2; z++)
            {
                AABB current;
                Octree final = {0};

                box_id = x + y * 2 + z * 4;
                current.min = box.min;
                current.max = box.max;
                if (x == 0)
                {
                    current.max.x = box_center.x;
                }
                else
                {
                    current.min.x = box_center.x;
                }
                if (y == 0)
                {
                    current.max.y = box_center.y;
                }
                else
                {
                    current.min.y = box_center.y;
                }
                if (z == 0)
                {
                    current.max.z = box_center.z;
                }
                else
                {
                    current.min.z = box_center.z;
                }

                if (octree_setup(&final, current, hitable, subdivide + 1))
                {
                    target->childs[box_id] = calloc(1, sizeof(Octree));
                    *target->childs[box_id] = final;
                    at_least_one = true;
                }
                else
                {
                    target->childs[box_id] = NULL;
                }
            }
        }
    }

    return at_least_one;
}
Octree *octree_create(HitableList *hitable, rt_float t_end, rt_float t_start)
{

    Octree *target;
    target = calloc(sizeof(Octree), 1);

    octree_setup(target, get_hitable_surrounding(hitable, t_end, t_start), hitable, 0);

    return target;
}

bool octree_hit(Ray r, rt_float t_min, rt_float *t_max, HitRecord *record, const Octree *self)
{

    return ray_cast_octree(self, r, t_min, *t_max, record);
}
