#include "color.h"

Color color_create(rt_float r, rt_float g, rt_float b)
{
    Color res;
    res.r = r;
    res.g = g;
    res.b = b;
    res.a = 1;
    return res;
}
