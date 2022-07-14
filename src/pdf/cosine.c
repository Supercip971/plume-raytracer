#include "cosine.h"
#include <stdio.h>

Pdf make_pdf_cosine(Onb onb)
{
    return (Pdf){
        .type = PDF_COSINE,
        .data.cosine = {
            .onb = onb,
        },
    };
}

rt_float cosine_pdf_value(Vec3 direction, const PdfData *self)
{
    rt_float cosine = vec3_dot(vec3_unit(direction), self->cosine.onb.w);
    return (cosine <= 0) ? 0 : cosine * M_1_PI;
}

Vec3 cosine_pdf_generate(const PdfData *self)
{
    Vec3 r = onb_local(&self->cosine.onb, random_vec3_cosine_direction());
    return r;
}
