#include "bvh.h"
#include "ds/list/hitable_list.h"
#include "ds/vec.h"
#include "math/aabb.h"
#include "shape/shape.h"

Object bvh_object(Bvh *self)
{
    return (Object){
        .data = self,
        .type = SHAPE_BVH,
    };
}

void bvh_init(Bvh *self)
{
    vec_init(&self->list);
    BvhEntry e = {};
    vec_push(&self->list, (e));
}

bool bvh_destroy(Bvh *self)
{
    vec_deinit(&self->list);
    return true;
}

void bvh_construct(Bvh *self, const HitableList *from)
{
    self->object_list = from;
    return bvh_construct_impl(&self->list, from);
}

bool bvh_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const Bvh *self)
{
    (void)time_start;
    (void)time_end;
    *output = self->list.data[0].box;
    return true;
}

#define STACK_SIZE 24

typedef struct
{
    HitRecord record;
    bool hitted;
} BvhHitRecord;

static inline BvhEntry bvh_entry(const Bvh *self, size_t i)
{
    return self->list.data[i];
}

static inline BvhHitRecord bvh_process_leaf(const Bvh *self, Ray r, int id, float cmin, float cmax)
{
    Object *object = &self->object_list->childs[id];

    HitRecord record = {};

    if (object_collide(r, cmin, cmax, &record, object))
    {
        return (BvhHitRecord){
            .record = record,
            .hitted = true,
        };
    }
    else
    {
        return (BvhHitRecord){
            .hitted = false,

        };
    }
}

static BvhHitRecord traverse_bvh_stack(const Bvh *self, Ray r, float cmin_f, float cmax_f)
{
    int stack[STACK_SIZE];

    int sp = 0;
    stack[sp] = 0;
    sp++;
    BvhHitRecord res = {};
    res.hitted = false;

    while (sp > 0)
    {
        sp--; // pop

        BvhEntry cur = bvh_entry(self, stack[sp]);
        if (cur.is_next_a_bvh == 0)
        {
            BvhHitRecord rec2 = bvh_process_leaf(self, r, cur.l, cmin_f, cmax_f);
            if (rec2.hitted)
            {
                cmax_f = rec2.record.t;
                res = rec2;
            }
            if (cur.r != 0)
            {
                rec2 = bvh_process_leaf(self, r, cur.r, cmin_f, cmax_f);
                if (rec2.hitted)
                {
                    cmax_f = rec2.record.t;
                    res = rec2;
                }
            }
        }
        else
        {
            BvhEntry lb = bvh_entry(self, cur.l);
            BvhEntry rb = bvh_entry(self, cur.r);

            if (aabb_hit(&lb.box, &r, cmin_f, cmax_f))
            {
                stack[sp] = cur.l;

                sp++;
            }
            if (aabb_hit(&rb.box, &r, cmin_f, cmax_f))
            {

                stack[sp] = cur.r;

                sp++;
            }
        }
    }
    return res;
}

bool bvh_hit(Ray r, rt_float t_min, rt_float t_max, HitRecord *record, const Bvh *self)
{
    BvhHitRecord rec = traverse_bvh_stack(self, r, t_min, t_max);
    if (rec.hitted)
    {
        *record = rec.record;
        return true;
    }

    return false;
}

Vec3 bvh_random(Vec3 origin, const Bvh *self)
{
    return hitable_random(origin, self->object_list);
}

rt_float bvh_pdf_value(Vec3 origin, Vec3 direction, const Bvh *self)
{
    return hitable_pdf_value(origin, direction, self->object_list);
}
