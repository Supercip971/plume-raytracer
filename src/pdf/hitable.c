#include "hitable.h"

Pdf make_pdf_hitable(Object *obj, Vec3 origin)
{
    Pdf self = {};
    self.data.hitable.origin = origin;
    self.data.hitable.obj = obj;
    self.type = PDF_HITABLE;

    return self;
}

rt_float pdf_hitable_value(Vec3 direction, const PdfData *self)
{
    return object_pdf_value(self->hitable.origin, direction, self->hitable.obj);
}

Vec3 hitable_pdf_generate(const PdfData *self)
{
    Vec3 r = object_random(self->hitable.origin, self->hitable.obj);

    return vec3_unit(r);
}
