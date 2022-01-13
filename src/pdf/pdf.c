#include "pdf.h"
#include <stdio.h>
#include "cosine.h"
#include "hitable.h"
#include "mixture.h"

rt_float pdf_value(Vec3 direction, const Pdf *self)
{
    switch (self->type)
    {
    case PDF_COSINE:
        return cosine_pdf_value(direction, &self->data);
    case PDF_HITABLE:
        return pdf_hitable_value(direction, &self->data);
    case PDF_MIXTURE:
        return mixture_pdf_value(direction, &self->data);
    default:
        return 0;
    }
}

Vec3 pdf_generate(const Pdf *self)
{
    switch (self->type)
    {
    case PDF_COSINE:
        return cosine_pdf_generate(&self->data);
    case PDF_HITABLE:
        return hitable_pdf_generate(&self->data);
    case PDF_MIXTURE:
        return mixture_pdf_generate(&self->data);
    default:
        return random_vec3_cosine_direction();
    }
}
