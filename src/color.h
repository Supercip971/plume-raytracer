#pragma once

typedef struct color
{
    double r;
    double g;
    double b;
    double a;
} Color;

Color color_create(double r, double g, double b);
