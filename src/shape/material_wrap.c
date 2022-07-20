#include "material_wrap.h"
#include <pdf/pdf.h>
#include <stdlib.h>
#include "shape/shape.h"

rt_float mwrap_pdf_value(Vec3 origin, Vec3 direction, const MaterialWrap *self)
{
    return object_pdf_value(origin,  direction, &self->target);
}

Vec3 mwrap_pdf_random(Vec3 origin, const MaterialWrap *self)
{
    return object_random(origin, &self->target);
}

Object material_wrap(Object target, Material material)
{
    AABB output;

    object_get_aabb(0, 1000, &output, &target) ; 
    MaterialWrap* data = malloc(sizeof(MaterialWrap));
    *data = (MaterialWrap) {
        .target = target,
        .material = material,
        .sbound = output,
    };

    return (Object){
        .data = data,
        .type = SHAPE_MATERIAL_WRAP,
    };
}
