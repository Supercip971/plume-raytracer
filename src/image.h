#pragma once

#include "color.h"
#include "stddef.h"
typedef struct image
{
    size_t width;
    size_t height;
    Color *converted_data; /* converted data used by the engine */
    void *source_data;     /* raw 8bit data (used by the implementation) */
} Image;
