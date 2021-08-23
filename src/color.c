#include "color.h"

Color color_create(double r, double g, double b)
{
    Color res;
    res.r = r;
    res.g = g;
    res.b = b;
    res.a = 1;
    return res;
}
