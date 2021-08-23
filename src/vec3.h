#pragma once
#include <math.h>
#include "color.h"

typedef struct vec3_t
{
    float x;
    float y;
    float z;
} Vec3;

/* may use float later (?)

  using inline for better performance with the compiler (less call instruction)*/
Vec3 vec3_create(float x, float y, float z);
Vec3 vec3_inv(Vec3 vec);
float vec3_squared_length(Vec3 vec);
float vec3_length(Vec3 vec);
Vec3 vec3_add(Vec3 vec1, Vec3 vec2);

Vec3 vec3_sub(Vec3 vec1, Vec3 vec2);

Vec3 vec3_div(Vec3 vec1, Vec3 vec2);

Vec3 vec3_mul(Vec3 vec1, Vec3 vec2);
Vec3 vec3_mul_val(Vec3 vec1, float x);

Vec3 vec3_div_val(Vec3 vec1, float x);

float vec3_dot(Vec3 vec1, Vec3 vec2);

Vec3 vec3_cross(Vec3 vec1, Vec3 vec2);

Vec3 vec3_unit(Vec3 vec);
Color vec_to_color(Vec3 from);
