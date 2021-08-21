#pragma once

typedef struct color
{
    float r;
    float g;
    float b;
    float a;
} Color;

Color color_create(float r, float g, float b);
