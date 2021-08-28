#ifndef IMAGE_H
#define IMAGE_H
#include <stddef.h>
#include <stdint.h>
#include "color.h"
typedef struct image
{
    int width;
    int height;
    Color *converted_data; /* converted data used by the engine */
    void *source_data;     /* raw 8bit data (used by the implementation) */
} Image;
#endif
