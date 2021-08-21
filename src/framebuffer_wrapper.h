#pragma once
#include <stdint.h>
#include "color.h"

struct raw_color
{
    uint8_t a;
    uint8_t b;
    uint8_t g;
    uint8_t r;
} __attribute__((packed));

#define color_f_to_int(color)             \
    (struct raw_color)                    \
    {                                     \
        .r = (uint8_t)(color.r * 255.99), \
        .g = (uint8_t)(color.g * 255.99), \
        .b = (uint8_t)(color.b * 255.99), \
        .a = (uint8_t)(color.a * 255.99)  \
    }

void render_loop();
