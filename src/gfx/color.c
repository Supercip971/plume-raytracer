#include "color.h"

Color color_create(rt_float r, rt_float g, rt_float b)
{
    Color res = {
        .r = r,
        .g = g,
        .b = b,
        .a = 1.f,
    };
    return res;
}
