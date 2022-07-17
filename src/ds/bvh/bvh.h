#ifndef BVH_H
#define BVH_H
/* todo: implement a better data structure */

#include <shape/hittable.h>
#include <ds/list/hitable_list.h>
#include <ds/vec.h>

typedef struct bvh_data
{
    int l;
    int r;
    bool is_next_a_bvh;
    AABB box;    
} BvhEntry;



typedef vec_t(BvhEntry) BvhList;

typedef struct 
{
    BvhList list;
    const HitableList* object_list;    
} Bvh;

void bvh_construct_impl(BvhList* self, const HitableList* from);
void bvh_construct(Bvh* self, const HitableList* from);


Object bvh_object(Bvh* self);

void bvh_init(Bvh* self);

bool bvh_destroy(Bvh*self);

bool bvh_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const Bvh *self);

bool bvh_hit(Ray r, rt_float t_min, rt_float t_max, HitRecord *record, const Bvh *self);


rt_float bvh_pdf_value(Vec3 origin, Vec3 direction, const Bvh *self);
Vec3 bvh_random(Vec3 origin, const Bvh *self);
static inline Object bvh_create_rec(const HitableList* root, int unused, int unused1)
{
    (void)unused;
    (void)unused1;
    Bvh * bvh = malloc(sizeof(Bvh));
    bvh_init(bvh);
    bvh_construct(bvh, root);

    return bvh_object(bvh);
}
#endif