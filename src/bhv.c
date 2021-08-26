#include "bhv.h"
#include <stdio.h>
#include <stdlib.h>
#include <utils.h>

static bool bhv_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const BhvData *self)
{
    *output = self->box;
    (void)time_start;
    (void)time_end;
    return true;
}
static bool bhv_hit(Ray r, rt_float t_min, rt_float t_max, HitRecord *record, const BhvData *self)
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

static int box_compare(Object *a, Object *b, int axis)
{
    AABB box_a;
    AABB box_b;

    if (!a->get_aabb(0, 0, &box_a, a->data) || !b->get_aabb(0, 0, &box_b, b->data))
    {
        printf("error: no bounding box for bhv (what ? this should not happend :thinking: \n");
        return false;
    }

    if (axis == 0)
    {
        return box_a.min.x > box_b.min.x;
    }
    else if (axis == 1)
    {
        return box_a.min.y > box_b.min.y;
    }
    else
    {
        return box_a.min.z > box_b.min.z;
    }
}
static bool bhv_destroy(BhvData *self)
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
        self->right.destroy(self->right.data);
    }
    if (destroy_right)
    {
        self->left.destroy(self->left.data);
    }
    free(self);
    return true;
}
/* we need to do that for qsort */
static int box_compare_x(Object *a, Object *b)
{
    return box_compare(a, b, 0);
}

static int box_compare_y(Object *a, Object *b)
{
    return box_compare(a, b, 1);
}

static int box_compare_z(Object *a, Object *b)
{
    return box_compare(a, b, 2);
}

static int take_axis(HitableList *list, size_t idx_start, size_t idx_end, rt_float time_start, rt_float time_end)
{
    rt_float x_span = 0, y_span = 0, z_span = 0;
    bool first = true;
    size_t i;
    AABB box;
    AABB result = {0};
    for (i = idx_start; i < idx_end; i++)
    {
        list->childs[i].get_aabb(time_start, time_end, &box, list->childs[i].data);
        if (first)
        {
            result = box;
        }
        else
        {
            result = aabb_surrounding(&result, &box);
        }

        first = false;
    }

    x_span = (result.max.x - result.min.x);
    y_span = (result.max.y - result.min.y);
    z_span = (result.max.z - result.min.z);

    if (x_span > y_span && x_span > z_span)
    {
        return 0;
    }
    if (y_span > z_span && y_span > z_span)
    {
        return 1;
    }
    return 2;
}
static void bhv_create_from_hitable(BhvData *target, HitableList *list, size_t idx_start, size_t idx_end, double t_start, double t_end)
{
    HitableList lst = *list;
    AABB box_left, box_right;
    size_t mid;

    size_t span = idx_end - idx_start;

    int axis = take_axis(&lst, idx_start, idx_end, t_start, t_end);
    if (span == 1)
    {
        target->right = lst.childs[idx_start];
        target->left = target->right;
    }
    else if (span == 2)
    {
        Object obj1 = lst.childs[idx_start];
        Object obj2 = lst.childs[idx_start + 1];
        if (box_compare(&obj1, &obj2, axis) == 0)
        {
            target->left = obj1;
            target->right = obj2;
        }
        else
        {
            target->left = obj2;
            target->right = obj1;
        }
    }
    else
    {
        switch (axis)
        {
        case 0:
        {
            qsort(((Object *)lst.childs) + idx_start, span, sizeof(Object), (__compar_fn_t)box_compare_x);
            break;
        }
        case 1:
        {
            qsort(((Object *)lst.childs) + idx_start, span, sizeof(Object), (__compar_fn_t)box_compare_y);
            break;
        }
        case 2:
        {
            qsort(((Object *)lst.childs) + idx_start, span, sizeof(Object), (__compar_fn_t)box_compare_z);
            break;
        }
        }

        mid = idx_start + (span / 2);

        target->left = bhv_create(&lst, idx_start, mid, t_start, t_end);
        target->right = bhv_create(&lst, mid, idx_end, t_start, t_end);
    }

    if (!target->left.get_aabb(t_start, t_end, &box_left, target->left.data) || !target->right.get_aabb(t_start, t_end, &box_right, target->right.data))
    {
        printf("error: no bounding box for bhv (what ? this should not happend :thinking: \n");
        return;
    }

    target->box = aabb_surrounding(&box_left, &box_right);
}

Object bhv_create(HitableList *list, size_t idx_start, size_t idx_end, double time_start, double time_end)
{
    BhvData *data = malloc(sizeof(BhvData));
    Object result;

    result.data = data;
    result.get_aabb = (ObjectGetAABB)bhv_get_aabb;
    result.collide = (ObjectCallback)bhv_hit;
    result.destroy = (ObjectDestroy)bhv_destroy;
    result.is_leaf = false;

    bhv_create_from_hitable(data, list, idx_start, idx_end, time_start, time_end);

    return result;
}
