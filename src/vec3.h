#pragma once
#include <immintrin.h>
#include <math.h>
#include <x86intrin.h>
#include "boolean.h"
#include "color.h"
#include "config.h"

typedef struct vec3_t
{
    rt_float x;
    rt_float y;
    rt_float z;
} Vec3;

Vec3 vec3_create(rt_float x, rt_float y, rt_float z);

Vec3 vec3_inv(Vec3 vec);

Vec3 vec3_add(Vec3 vec1, Vec3 vec2);

Vec3 vec3_sub(Vec3 vec1, Vec3 vec2);

Vec3 vec3_div(Vec3 vec1, Vec3 vec2);
Vec3 vec3_div_val(Vec3 vec1, rt_float x);

Vec3 vec3_mul(Vec3 vec1, Vec3 vec2);
Vec3 vec3_mul_val(Vec3 vec1, rt_float x);

rt_float vec3_dot(Vec3 vec1, Vec3 vec2);

Vec3 vec3_cross(Vec3 vec1, Vec3 vec2);

Vec3 vec3_unit(Vec3 vec);

rt_float vec3_squared_length(Vec3 vec);
rt_float vec3_length(Vec3 vec);

bool is_vec3_near_zero(Vec3 vec);

Vec3 random_vec3_unit(void);
Vec3 random_vec3_unit_in_disk(void);
Vec3 random_vec3_in_hemisphere(Vec3 normal);

Vec3 reflect(Vec3 vec1, Vec3 vec2);

Vec3 refract(Vec3 vec1, Vec3 vec2, rt_float ni_over_nt);

Color vec_to_color(Vec3 from);
Vec3 vec_from_color(Color col);
