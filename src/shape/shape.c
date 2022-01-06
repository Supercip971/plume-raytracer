#include "shape.h"
#include "../bvh.h"
#include "../hitable.h"
#include "Sphere.h"
#include "aa_rec.h"
#include "box.h"
#include "moving_sphere.h"
#include "transform.h"
#include "translate.h"

bool object_collide(Ray r, rt_float t_min, rt_float t_max, HitRecord *record, Object *self)
{
    switch (self->type)
    {
    case SHAPE_BVH:
        return bvh_hit(r, t_min, t_max, record, self->data);
    case SHAPE_HITABLE_LIST:
        return hitable_list_call_all(r, t_min, t_max, record, self->data);
    case SHAPE_TRANSFORM:
        return hit_transform_object_callback(r, t_min, t_max, record, self->data);
    case SHAPE_BOX:
        return hit_box_object_callback(r, t_min, t_max, record, self->data);
    case SHAPE_MOVING_SPHERE:
        return hit_moving_sphere_object_callback(r, t_min, t_max, record, self->data);
    case SHAPE_SPHERE:
        return hit_sphere_object_callback(r, t_min, t_max, record, self->data);
    case SHAPE_TRANSLATE:
        return hit_translate_object_callback(r, t_min, t_max, record, self->data);
    case SHAPE_AAREC_XY:
        return hit_aaxyrect_callback(r, t_min, t_max, record, self->data);
    case SHAPE_AAREC_XZ:
        return hit_aaxzrect_callback(r, t_min, t_max, record, self->data);
    case SHAPE_AAREC_YZ:
        return hit_aayzrect_callback(r, t_min, t_max, record, self->data);

    default:
        return false;
    }
}
bool object_get_aabb(rt_float time_start, rt_float time_end, AABB *output, Object *self)
{
    switch (self->type)
    {
    case SHAPE_BVH:
        return bvh_get_aabb(time_start, time_end, output, self->data);
    case SHAPE_HITABLE_LIST:
        return hitable_get_aabb(time_start, time_end, output, self->data);
    case SHAPE_TRANSFORM:
        return transform_get_aabb(time_start, time_end, output, self->data);

    case SHAPE_BOX:
        return box_get_aabb(time_start, time_end, output, self->data);
    case SHAPE_MOVING_SPHERE:
        return moving_sphere_get_aabb(time_start, time_end, output, self->data);
    case SHAPE_SPHERE:
        return sphere_get_aabb(time_start, time_end, output, self->data);
    case SHAPE_TRANSLATE:
        return translate_get_aabb(time_start, time_end, output, self->data);
    case SHAPE_AAREC_XY:
        return aaxyrect_get_aabb(time_start, time_end, output, self->data);
    case SHAPE_AAREC_XZ:
        return aaxzrect_get_aabb(time_start, time_end, output, self->data);
    case SHAPE_AAREC_YZ:
        return aayzrect_get_aabb(time_start, time_end, output, self->data);

    default:
        return false;
    }
}
bool object_destroy(Object *self)
{
    switch (self->type)
    {
    case SHAPE_HITABLE_LIST:
        return hitable_list_destroy(self->data);
    case SHAPE_BVH:
        return bvh_destroy(self->data);
    case SHAPE_BOX:
        return box_destroy(self->data);
    case SHAPE_TRANSLATE:
        return translate_destroy(self->data);
    case SHAPE_TRANSFORM:
        return transform_destroy(self->data);
    default:
        return false;
    }
}
