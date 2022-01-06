#include "box.h"
#include "aa_rec.h"

bool box_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const Box *self)
{
    (void)time_start;
    (void)time_end;
    *output = (self->self_bounding_box);
    return true;
}

bool box_destroy(Box *self)
{
    size_t i;
    for (i = 0; i < self->list.child_count; i++)
    {

        object_destroy(&self->list.childs[i]);
        if (self->list.childs[i].data)
        {
            free(self->list.childs[i].data);
        }
    }
    return true;
}
FLATTEN bool hit_box_object_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const Box *self)
{
    return hit_call_all_list(&self->list, ray, t_min, t_max, record);
}

Object box_create(Vec3 posa, Vec3 posb, Material box_material)
{
    Object res;
    Box *b = malloc(sizeof(Box));

    b->list.child_count = 0;
    b->self_bounding_box.min = posa;
    b->self_bounding_box.max = posb;

    res.data = b;
    res.type = SHAPE_BOX;
    res.is_leaf = true;

    add_hitable_list(&b->list, aaxyrect_create(posa.x, posb.x, posa.y, posb.y, posb.z, box_material));
    add_hitable_list(&b->list, aaxyrect_create(posa.x, posb.x, posa.y, posb.y, posa.z, box_material));

    add_hitable_list(&b->list, aaxzrect_create(posa.x, posb.x, posa.z, posb.z, posb.y, box_material));
    add_hitable_list(&b->list, aaxzrect_create(posa.x, posb.x, posa.z, posb.z, posa.y, box_material));

    add_hitable_list(&b->list, aayzrect_create(posa.y, posb.y, posa.z, posb.z, posb.x, box_material));
    add_hitable_list(&b->list, aayzrect_create(posa.y, posb.y, posa.z, posb.z, posa.x, box_material));

    return res;
}
