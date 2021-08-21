#include "color.h"

Color color_create(float r, float g, float b)
{
    Color res;
    res.r = r;
    res.g = g;
    res.b = b;
    res.a = 1;
    return res;
}
