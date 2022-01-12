#ifndef MIXTURE_H
#define MIXTURE_H
#include "pdf.h"

rt_float mixture_pdf_value(Vec3 direction, const PdfData *self);

Vec3 mixture_pdf_generate(const PdfData *self);
Pdf make_mixture_pdf(Pdf *first, Pdf *second);

#endif /* A6DCCD83_51BF_4070_B721_AC6C037832A4 */
