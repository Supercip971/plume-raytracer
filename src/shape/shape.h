#ifndef __SHAPE_H__
#define __SHAPE_H__

#include <math/aabb.h>

typedef enum
{
    SHAPE_NONE,
    SHAPE_BOX,
    SHAPE_AAREC_XY,
    SHAPE_AAREC_XZ,
    SHAPE_AAREC_YZ,
    SHAPE_MOVING_SPHERE,
    SHAPE_SPHERE,
    SHAPE_TRANSLATE,
    SHAPE_BVH,
    SHAPE_HITABLE_LIST,
    SHAPE_TRANSFORM,
    SHAPE_CONSTANT_MEDIUM,
    SHAPE_MATERIAL_WRAP,
    SHAPE_TRIANGLE,
    

} ShapeTypes;

typedef struct
{
    void *data;
    ShapeTypes type;
} Object;

typedef struct hit_record HitRecord;


bool object_collide(Ray r, rt_float t_min, rt_float t_max, HitRecord *record, Object *self);

bool object_get_aabb(rt_float time_start, rt_float time_end, AABB *output, Object *self);

bool object_destroy(Object *self);

rt_float object_pdf_value(Vec3 origin, Vec3 direction, const Object *self);

Vec3 object_random(Vec3 origin, const Object *self);

#endif /* B0BAA9D8_C3FE_4855_9A2C_59A9028F887C */
