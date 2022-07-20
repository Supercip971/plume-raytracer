#include <shape/hittable.h>
#include <shape/shape.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils.h>
#include "bvh.h"
#include "ds/list/hitable_list.h"
#include "math/aabb.h"

typedef struct
{
    BvhEntry entry;
    void *next_l;
    void *next_r;

} ElementOnList;

static ElementOnList *element_copy(ElementOnList *from)
{

    ElementOnList *d = malloc(sizeof(ElementOnList));
    *d = *from;
    return d;
}

static Vec3 aabb_centroid(const AABB *a)
{
    Vec3 a_center = vec3_add(a->min, vec3_mul_val(vec3_sub(a->max, a->min), 0.5));

    return a_center;
}

typedef vec_t(ElementOnList) tempBvhList;

static BvhEntry bvh_make_mesh_fusion(BvhEntry left, BvhEntry right)
{

    BvhEntry curr = {
        .is_next_a_bvh = false,
        .l = left.l,
        .r = right.l,
        //    .parent = 0,
        .box = aabb_surrounding(&left.box, &right.box),
    };

    return curr;
}

static BvhEntry bvh_make_from_temp_list(BvhList *self, ElementOnList *element)
{
    if (!element->entry.is_next_a_bvh)
    {
        element->entry.r = 0;
        return element->entry;
    }

    BvhEntry left = bvh_make_from_temp_list(self, element->next_l);
    BvhEntry right = bvh_make_from_temp_list(self, element->next_r);

    if (!left.is_next_a_bvh && !right.is_next_a_bvh && left.r == 0 && right.r == 0)
    {
        return bvh_make_mesh_fusion(left, right);
    }

    int l = self->length;

    vec_push(self, left);

    int r = l + 1;

    vec_push(self, right);

    // bvh_update_parents(self, &self->data[l], l);
    // bvh_update_parents(self, &self->data[r], r);
    BvhEntry curr = {
        .is_next_a_bvh = true,
        .l = l,
        .r = r,
        //    .parent = 0,
        .box = aabb_surrounding(&left.box, &right.box),
    };

    return curr;
}

int bvh_dump(BvhList *self, BvhEntry *entry, int depth);
int bvh_dump(BvhList *self, BvhEntry *entry, int depth)
{

    int maxdepth = depth;
#define TAB()                       \
    for (int i = 0; i < depth; i++) \
    {                               \
        printf("\t");               \
    }
    TAB();
    printf("entry[%i] \n", depth);
    TAB();
    printf("{%f,%f,%f} {%f,%f,%f}\n", entry->box.min.x, entry->box.min.y, entry->box.min.z, entry->box.max.x, entry->box.max.y, entry->box.max.z);
    TAB();
    if (!entry->is_next_a_bvh)
    {
        printf("bvh: %i\n", entry->l);
    }
    else
    {

        printf("- l: %i \n", entry->l);
        int c = bvh_dump(self, &self->data[entry->l], depth + 1);

        TAB();
        printf("- r: %i \n", entry->r);
        int c2 = bvh_dump(self, &self->data[entry->r], depth + 1);

        maxdepth = rt_max(rt_max(c, c2), maxdepth);
    }

    return maxdepth;
}

void sbvh_init(BvhList *self, int entry_id, HitableList *scene);
void sbvh_init(BvhList *self, int entry_id, HitableList *scene)
{
    BvhEntry *entry = &self->data[entry_id];
    if (!entry->is_next_a_bvh && entry->r != 0)
    {
        BvhEntry left = self->data[entry->l];

        BvhEntry right = self->data[entry->r];
        (void)left;
        (void)right;
    }
    else
    {
        sbvh_init(self, entry->l, scene);
        sbvh_init(self, entry->r, scene);
    }
}
// probably don't know what this does in 1 month
// this tries to create a bvh using a linear memory (usable for gpu) without any pointers
// this is probably really inefficient:
// x + x/2 + x/4 ...
// n + (n)
// but as we are using it once, I may not want to make it faster (for now)

typedef enum
{
    BDIM_X,
    BDIM_Y,
    BDIM_Z,
} DimensionBvh;

static DimensionBvh pick_dim(Vec3 a, Vec3 b)
{
    float dx = fabs(a.x - b.x);
    float dy = fabs(a.y - b.y);
    float dz = fabs(a.z - b.z);

    if (dx > dy && dx > dz)
    {
        return BDIM_X;
    }
    else if (dy > dx && dy > dz)
    {
        return BDIM_Y;
    }
    return BDIM_Z;
}

static bool is_vec3_dim_superior(Vec3 point, Vec3 maxc, Vec3 minc, DimensionBvh dim)
{

    if (dim == BDIM_X)
    {
        float center = (maxc.x + minc.x) / 2;
        return point.x >= center;
    }
    else if (dim == BDIM_Y)
    {
        float center = (maxc.y + minc.y) / 2;

        return point.y >= center;
    }
    float center = (maxc.z + minc.z) / 2;

    return point.z >= center;
}

static ElementOnList bvh_init_rec(tempBvhList *list, BvhList *tlist, int depth)
{
    if (list->length == 0)
    {
        abort();
    }
    if (list->length == 1)
    {
        return list->data[0];
    }
    ElementOnList res = {};
    tempBvhList vl;
    tempBvhList vr;
    vec_init(&vr);
    vec_init(&vl);

    Vec3 max_p = (list->data[0].entry.box.max);
    Vec3 min_p = (list->data[0].entry.box.min);

    Vec3 min_c = aabb_centroid(&list->data->entry.box);
    Vec3 max_c = aabb_centroid(&list->data->entry.box);
    for (int i = 0; i < list->length; i++)
    {
        max_p = vec3_max(max_p, list->data[i].entry.box.max);
        min_p = vec3_min(min_p, list->data[i].entry.box.min);

        max_c = vec3_max(max_p, aabb_centroid(&list->data[i].entry.box));
        min_c = vec3_min(min_p, aabb_centroid(&list->data[i].entry.box));
    }

    DimensionBvh dim = pick_dim(min_c, max_c);

    for (int i = 0; i < list->length; i++)
    {
        ElementOnList curr = list->data[i];

        if (is_vec3_dim_superior(aabb_centroid(&curr.entry.box), max_c, min_c, dim))
        {
            vec_push(&vr, curr);
        }
        else
        {
            vec_push(&vl, curr);
        }
    }

    //  printf("step[%i] (%i) l: %i r: %i \n", depth, list->length, vl.length, vr.length);
    //  printf("%f %f %f - %f %f %f\n", min_p.x, min_p.y, min_p.z, max_p.x, max_p.y, max_p.z);
    // generally when we have a lot of point in the same space, just random push, maybe I'll do something more
    // intelligent but this is a really rare edge case
    if (vl.length == 0 || vr.length == 0)
    {
        vec_clear(&vr);
        vec_clear(&vl);
        for (int i = 0; i < list->length; i++)
        {

            ElementOnList curr = list->data[i];
            if (i % 2)
            {
                vec_push(&vr, curr);
            }
            else
            {
                vec_push(&vl, curr);
            }
        }
    }
    res.entry.box = (AABB){min_p, max_p};
    res.entry.is_next_a_bvh = true;
    ElementOnList l = bvh_init_rec(&vl, tlist, 1 + depth);

    res.next_l = element_copy(&l);

    vec_deinit(&vl);
    ElementOnList r = bvh_init_rec(&vr, tlist, 1 + depth);

    res.next_r = element_copy(&r);

    vec_deinit(&vr);

    return res;
}
void bvh_construct_impl(BvhList *self, const HitableList *target)
{

    tempBvhList curr;
    vec_init(&curr);

    // first list creation
    for (size_t i = 0; i < target->child_count; i++)
    {
        Object m = target->childs[i];
        BvhEntry entry = {
            .is_next_a_bvh = false,
            .l = i,
        };

        object_get_aabb(0, 1, &entry.box, &m);
        printf("entry: {%f %f %f} {%f %f %f} \n", entry.box.min.x, entry.box.min.y, entry.box.min.z, entry.box.max.x, entry.box.max.y, entry.box.max.z);
        ElementOnList on_list = {
            .entry = entry,
        };

        vec_push(&curr, on_list);
    }

    BvhEntry start = {};

    ElementOnList element = bvh_init_rec(&curr, self, 0);
    start = bvh_make_from_temp_list(self, &element);

    self->data[0] = start;
    int i = bvh_dump(self, self->data, 0);
    printf("bvh depth: %i \n", i);
    printf("bvh size: %lu \n", self->length * sizeof(BvhEntry));
}
