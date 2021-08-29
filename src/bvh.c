#include <stdio.h>
#include <stdlib.h>
#include <utils.h>
#include "bvh.h"

static bool bvh_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const bvhData *self)
{
    *output = self->box;
    (void)time_start;
    (void)time_end;
    return true;
}
static bool bvh_hit(Ray r, rt_float t_min, rt_float t_max, HitRecord *record, const bvhData *self)
{
    bool hit_left, hit_right;

    if (!aabb_hit(&self->box, &r, t_min, t_max))
    {
        return false;
    }

    hit_left = self->left.collide(r, t_min, t_max, record, self->left.data);
    hit_right = self->right.collide(r, t_min, hit_left ? record->t : t_max, record, self->right.data);

    return hit_left || hit_right;
}

static bool bvh_destroy(bvhData *self)
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

    if (destroy_left && self->left.destroy != NULL)
    {
        self->left.destroy(self->left.data);
    }
    if (destroy_right && self->right.destroy != NULL)
    {
        self->right.destroy(self->right.data);
    }
    free(self);
    return true;
}

static Object bvh_init(bvhData **out_bvh)
{

    bvhData *data = malloc(sizeof(bvhData));
    Object result;
    result.data = data;
    result.get_aabb = (ObjectGetAABB)bvh_get_aabb;
    result.collide = (ObjectCallback)bvh_hit;
    result.destroy = (ObjectDestroy)bvh_destroy;
    result.is_leaf = false;
    *out_bvh = data;
    return result;
}

static rt_float bvh_distance(Object *left, Object *right, rt_float tstart, rt_float tend)
{
    AABB a, b;
    Vec3 a_center, b_center;
    left->get_aabb(tstart, tend, &a, left->data);
    right->get_aabb(tstart, tend, &b, right->data);

    a_center = vec3_add(a.min, vec3_mul_val(vec3_sub(a.max, a.min), 0.5));
    b_center = vec3_add(b.min, vec3_mul_val(vec3_sub(b.max, b.min), 0.5));

    return vec3_length(vec3_sub(b_center, a_center));
}

void bvh_create_rec(HitableList *list, rt_float tstart, rt_float tend)
{

    size_t i, j;
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

        for (i = 0; i < list->child_count; i++)
        {
            for (j = 0; j < list->child_count; j++)
            {
                if (i == j)
                {
                    continue;
                }
                dist = bvh_distance(&list->childs[i], &list->childs[j], tstart, tend);

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
        left->get_aabb(tstart, tend, &l_aa, left->data);
        right->get_aabb(tstart, tend, &r_aa, right->data);
        result_bvh->box = aabb_surrounding(&l_aa, &r_aa);

        hit_remove_object(list, *right);
        hit_remove_object(list, *left);

        add_hitable_list(list, result_obj);
    }
}