#ifndef _HITABLE_H_
#define _HITABLE_H_
#include "../vec3.h"
#include "pdf.h"

Pdf make_pdf_hitable(Object *obj, Vec3 origin);

rt_float pdf_hitable_value(Vec3 direction, const PdfData *self);

Vec3 hitable_pdf_generate(const PdfData *self);
#endif /* HITABLE H*/
