#include <stdio.h>
#include <stdlib.h>
#include <utils.h>
#include "bvh.h"
#include <shape/shape.h>
#include <shape/hittable.h>

bvhData *bvh_blocks = NULL;
int idx = 0;

// this will be rewritten for a cache-friendly implementation
static bvhData *alloc_bvh_data(void)
{
    if (bvh_blocks == NULL)
    {
        bvh_blocks = malloc(sizeof(bvhData) * 512);
    }

    idx++;
    if (idx > 511)
    {
        return malloc(sizeof(bvhData));
    }
    return &bvh_blocks[idx];
}

static void free_bvh_data(bvhData *v)
{
    idx--;
    if (idx == 0)
    {
        free(bvh_blocks);
    }
    (void)v;
}

bool bvh_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const bvhData *self)
{
    *output = self->box;
    (void)time_start;
    (void)time_end;
    return true;
}

bool bvh_hit(Ray r, rt_float t_min, rt_float t_max, HitRecord *record, const bvhData *self)
{
    if (!aabb_hit(&self->box, &r, t_min, t_max))
    {
        return false;
    }

    AABB box_left, box_right;

    object_get_aabb(0, 1, &box_left, (Object *)&self->left);
    object_get_aabb(0, 1, &box_right, (Object *)&self->right);

    bool hit_left = false, hit_right = false;

    if (aabb_hit(&box_left, &r, t_min, t_max))
    {
        hit_left = object_collide(r, t_min, t_max, record, (Object *)&self->left);
    }
    if (aabb_hit(&box_right, &r, t_min, hit_left ? record->t : t_max))
    {
        hit_right = object_collide(r, t_min, hit_left ? record->t : t_max, record, (Object *)&self->right);
    }

    return hit_left | hit_right;
}

bool bvh_destroy(bvhData *self)
{

    bool destroy_left = false;
    bool destroy_right = false;
    if (self->right.data)
    {
        destroy_right = true;
    }
    if (self->left.data && self->right.data != self->left.data)
    {
        destroy_left = true;
    }

    if (destroy_left)
    {
        object_destroy(&self->left);
    }
    if (destroy_right)
    {
        object_destroy(&self->right);
    }

    free_bvh_data(self);
    return true;
}

static Object bvh_init(bvhData **out_bvh)
{
    bvhData *data = alloc_bvh_data();

    Object result = {
        .data = data,
        .type = SHAPE_BVH,
        .is_leaf = false,
    };

    *out_bvh = data;
    return result;
}

static rt_float bvh_distance(Object *left, Object *right, rt_float tstart, rt_float tend, bool *intersect)
{
    AABB a, b;
    object_get_aabb(tstart, tend, &a, left);
    object_get_aabb(tstart, tend, &b, right);

    *intersect = aabb_intersect(&a, &b);

    Vec3 a_center = vec3_add(a.min, vec3_mul_val(vec3_sub(a.max, a.min), 0.5));
    Vec3 b_center = vec3_add(b.min, vec3_mul_val(vec3_sub(b.max, b.min), 0.5));

    return vec3_squared_length(vec3_sub(b_center, a_center));
}

void bvh_create_rec(HitableList *list, rt_float tstart, rt_float tend)
{

    while (list->child_count > 1)
    {
        Object *left = NULL;
        Object *right = NULL;
        rt_float best_dist = 10000000000;
        rt_float dist;
        bvhData *result_bvh;
        Object result_obj;
        AABB l_aa;
        AABB r_aa;

        for (size_t i = 0; i < list->child_count; i++)
        {
            for (size_t j = 0; j < list->child_count; j++)
            {
                if (i == j)
                {
                    continue;
                }
                bool intersect = false;
                dist = bvh_distance(&list->childs[i], &list->childs[j], tstart, tend, &intersect);
                if (dist < best_dist)
                {
                    best_dist = dist;
                    left = &list->childs[i];
                    right = &list->childs[j];
                }
            }
        }
        if (!left || !right)
        {
            return;
        }
        result_obj = bvh_init(&result_bvh);

        result_bvh->right = *right;
        result_bvh->left = *left;
        object_get_aabb(tstart, tend, &l_aa, left);
        object_get_aabb(tstart, tend, &r_aa, right);
        result_bvh->has_conflict = aabb_intersect(&l_aa, &r_aa);

        result_bvh->box = aabb_surrounding(&l_aa, &r_aa);

        hit_remove_object(list, *right);
        hit_remove_object(list, *left);

        add_hitable_list(list, result_obj);
    }
}

rt_float bvh_pdf_value(Vec3 origin, Vec3 direction, const bvhData *self)
{
    rt_float val_left = object_pdf_value(origin, direction, (Object *)&self->left);
    rt_float val_right = object_pdf_value(origin, direction, (Object *)&self->right);

    return (val_left + val_right) / 2;
}

Vec3 bvh_random(Vec3 origin, const bvhData *self)
{
    if (random_rt_float() > 0.5)
    {
        return object_random(origin, &self->left);
    }
    else
    {
        return object_random(origin, &self->right);
    }
}
