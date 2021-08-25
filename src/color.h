#pragma once

#include "config.h"
typedef struct color
{
    rt_float r;
    rt_float g;
    rt_float b;
    rt_float a;
} Color;

Color color_create(rt_float r, rt_float g, rt_float b);
