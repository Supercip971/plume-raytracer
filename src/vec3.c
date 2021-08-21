#include "vec3.h"
Vec3 vec3_create(float x, float y, float z)
{
    Vec3 res;
    res.x = x;
    res.y = y;
    res.z = z;
    return res;
}

Vec3 vec3_inv(Vec3 vec)
{
    return vec3_create(-(vec).x, -(vec).y, -(vec).z);
}

float vec3_squared_length(Vec3 vec)
{
    return ((vec).x * (vec).x + (vec).y * (vec).y + (vec).z * (vec).z);
}

float vec3_length(Vec3 vec)
{
    return sqrt(vec3_squared_length(vec));
}
Vec3 vec3_add(Vec3 vec1, Vec3 vec2)
{
    return vec3_create(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z);
}

Vec3 vec3_sub(Vec3 vec1, Vec3 vec2)
{
    return vec3_create(vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z);
}

Vec3 vec3_div(Vec3 vec1, Vec3 vec2)
{
    return vec3_create(vec1.x / vec2.x, vec1.y / vec2.y, vec1.z / vec2.z);
}

Vec3 vec3_mul(Vec3 vec1, Vec3 vec2)
{
    return vec3_create(vec1.x * vec2.x, vec1.y * vec2.y, vec1.z * vec2.z);
}
Vec3 vec3_mul_val(Vec3 vec1, float x)
{
    return vec3_create(vec1.x * x, vec1.y * x, vec1.z * x);
}

Vec3 vec3_div_val(Vec3 vec1, float x)
{
    return vec3_create(vec1.x / x, vec1.y / x, vec1.z / x);
}

float vec3_dot(Vec3 vec1, Vec3 vec2)
{
    return (vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z);
}

Vec3 vec3_cross(Vec3 vec1, Vec3 vec2)
{
    return vec3_create(vec1.y * vec2.z - vec1.z * vec2.y,
                       -(vec1.x * vec2.z - vec1.z * vec2.x),
                       vec1.y * vec2.x - vec1.x * vec2.y);
}

Vec3 vec3_unit(Vec3 vec)
{
    return vec3_div_val(vec, vec3_length(vec));
}

Color vec_to_color(Vec3 from)
{
    return color_create(
        from.x,
        from.y,
        from.z);
}
