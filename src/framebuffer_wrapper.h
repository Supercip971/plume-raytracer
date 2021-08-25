#pragma once
#include <stdint.h>
#include "color.h"

void render_loop(void);

#define set_pixel(FB, X, Y, WIDTH, COLOR) \
    (FB)[X + Y * WIDTH] = COLOR

#define get_pixel(FB, X, Y, WIDTH) \
    (FB)[X + Y * WIDTH]
