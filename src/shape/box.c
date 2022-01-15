#include "box.h"
#include "../bvh.h"
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
    hit_destroy_all_objects(&self->list);
    return true;
}
FLATTEN bool hit_box_object_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const Box *self)
{
    return object_collide(ray, t_min, t_max, record, (Object *)&self->list);
}

Object box_create(Vec3 posa, Vec3 posb, Material box_material)
{
    Object res;
    Box *b = malloc(sizeof(Box));

    b->self_bounding_box = aabb_create(posa, posb);

    res.data = b;
    res.type = SHAPE_BOX;
    res.is_leaf = true;
    b->list = create_hitable_list();

    add_hitable_object(&b->list, aaxyrect_create(posa.x, posb.x, posa.y, posb.y, posb.z, box_material));
    add_hitable_object(&b->list, aaxyrect_create(posa.x, posb.x, posa.y, posb.y, posa.z, box_material));

    add_hitable_object(&b->list, aaxzrect_create(posa.x, posb.x, posa.z, posb.z, posb.y, box_material));
    add_hitable_object(&b->list, aaxzrect_create(posa.x, posb.x, posa.z, posb.z, posa.y, box_material));

    add_hitable_object(&b->list, aayzrect_create(posa.y, posb.y, posa.z, posb.z, posb.x, box_material));
    add_hitable_object(&b->list, aayzrect_create(posa.y, posb.y, posa.z, posb.z, posa.x, box_material));

    bvh_create_rec(b->list.data, 0, 1);
    return res;
}

rt_float box_pdf_value(Vec3 origin, Vec3 direction, const Box *self)
{
    return object_pdf_value(origin, direction, &self->list);
}
Vec3 box_random(Vec3 origin, const Box *self)
{
    return object_random(origin, &self->list);
}
