#include "cosine.h"
#include <stdio.h>
Pdf make_pdf_cosine(Onb onb)
{
    Pdf pdf;
    pdf.type = PDF_COSINE;
    pdf.data.cosine.onb = onb;

    return pdf;
}

rt_float cosine_pdf_value(Vec3 direction, const PdfData *self)
{
    rt_float cosine = vec3_dot(vec3_unit(direction), self->cosine.onb.w);

    if (((volatile rt_float)cosine == cosine + 1))
    {
        printf("fuck\n");
    }
    return cosine < 0 ? 0 : cosine / M_PI;
}

Vec3 cosine_pdf_generate(const PdfData *self)
{
    return onb_local(&self->cosine.onb, random_vec3_cosine_direction());
}
