#pragma once
#include <math.h>
#include "color.h"

typedef struct vec3_t
{
    double x;
    double y;
    double z;
} Vec3;

/* may use double later (?)

  using inline for better performance with the compiler (less call instruction)*/
Vec3 vec3_create(double x, double y, double z);
Vec3 vec3_inv(Vec3 vec);
double vec3_squared_length(Vec3 vec);
double vec3_length(Vec3 vec);
Vec3 vec3_add(Vec3 vec1, Vec3 vec2);

Vec3 vec3_sub(Vec3 vec1, Vec3 vec2);

Vec3 vec3_div(Vec3 vec1, Vec3 vec2);

Vec3 vec3_mul(Vec3 vec1, Vec3 vec2);
Vec3 vec3_mul_val(Vec3 vec1, double x);

Vec3 vec3_div_val(Vec3 vec1, double x);

double vec3_dot(Vec3 vec1, Vec3 vec2);

Vec3 vec3_cross(Vec3 vec1, Vec3 vec2);

Vec3 vec3_unit(Vec3 vec);
Color vec_to_color(Vec3 from);

Vec3 random_vec3_unit(void);
