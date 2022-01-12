#ifndef PDF_H
#define PDF_H
#include "../onb.h"
#include "../shape/shape.h"
#include "../utils.h"
#include "../vec3.h"

typedef enum
{
    PDF_NONE,
    PDF_COSINE,
    PDF_HITABLE,
    PDF_MIXTURE,
} PdfTypes;

/*
    instead of using a void* data this is using an union
    this is because we may create this structure every pixel and using an allocation is heavy
*/
typedef struct pdf_structure Pdf;
union pdf_data
{
    struct
    {
        Onb onb;
    } cosine;
    struct
    {
        Object *obj;
        Vec3 origin;
    } hitable;

    struct
    {
        Pdf *first;
        Pdf *second;
    } mixture;
};

typedef union pdf_data PdfData;
struct pdf_structure
{
    PdfTypes type;
    PdfData data;
};

rt_float pdf_value(Vec3 direction, const Pdf *self);

Vec3 pdf_generate(const Pdf *self);
#endif
