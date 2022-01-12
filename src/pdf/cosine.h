#ifndef COSINE_H
#define COSINE_H
#include "../onb.h"
#include "pdf.h"

Pdf make_pdf_cosine(Onb onb);

rt_float cosine_pdf_value(Vec3 direction, const PdfData*self);

Vec3 cosine_pdf_generate(const PdfData *self);
#endif
