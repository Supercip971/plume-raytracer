#include "pdf.h"
#include <stdio.h>
#include <pdf/cosine.h>
#include <pdf/hitable.h>
#include <pdf/mixture.h>

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
        return vec3_create(1, 0, 0);
    }
}
