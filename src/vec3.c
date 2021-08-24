#include "vec3.h"
#include "utils.h"
Vec3 vec3_create(double x, double y, double z)
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

double vec3_squared_length(Vec3 vec)
{
    return ((vec).x * (vec).x + (vec).y * (vec).y + (vec).z * (vec).z);
}

double vec3_length(Vec3 vec)
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
Vec3 vec3_mul_val(Vec3 vec1, double x)
{
    return vec3_create(vec1.x * x, vec1.y * x, vec1.z * x);
}

Vec3 vec3_div_val(Vec3 vec1, double x)
{
    return vec3_create(vec1.x / x, vec1.y / x, vec1.z / x);
}

double vec3_dot(Vec3 vec1, Vec3 vec2)
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

Vec3 random_vec3_unit(void)
{
    Vec3 vec;
    do
    {
        vec = vec3_sub(vec3_mul_val(vec3_create(random_double(), random_double(), random_double()), 2), vec3_create(1, 1, 1));
    } while (vec3_squared_length(vec) >= 1.0);
    return vec;
}

Vec3 reflect(Vec3 vec1, Vec3 vec2)
{
    return vec3_sub(vec1, vec3_mul_val(vec2, (vec3_dot(vec1, vec2) * 2.0)));
}
