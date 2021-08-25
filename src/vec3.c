#include "vec3.h"
#include <immintrin.h>
#include <mmintrin.h>
#include <smmintrin.h>
#include <x86intrin.h>
#include <xmmintrin.h>
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
    return vec3_create(-vec.x, -vec.y, -vec.z);
}

double vec3_squared_length(Vec3 vec)
{
    return (vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

double vec3_length(Vec3 vec)
{
    return fast_sqrt(vec3_squared_length(vec));
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
                       (vec1.z * vec2.x - vec1.x * vec2.z),
                       vec1.x * vec2.y - vec1.y * vec2.x);
}

#ifdef NO_FAST

Vec3 vec3_unit(Vec3 vec)
{
    return vec3_div_val(vec, vec3_lengthvec);
}

#else

/* vec / sqrt(vecx * vecx + vecy * vecy + vecz * vecz)
// vec * 1/sqrt()
// vec * Q_rdqrt()*/
Vec3 vec3_unit(Vec3 vec)
{
    return vec3_mul_val(vec, Q_rsqrt(vec3_squared_length(vec)));
}

#endif

Vec3 random_vec3_unit(void)
{

    while (true)
    {
        Vec3 p = vec3_create(random_double() * 2 - 1, random_double() * 2 - 1, random_double() * 2 - 1);
        if (vec3_squared_length(p) >= 1)
            continue;
        return p;
    }
}

Vec3 random_vec3_unit_in_disk(void)
{
    while (true)
    {
        Vec3 p = vec3_create(random_double() * 2 - 1, random_double() * 2 - 1, 0);
        if (vec3_squared_length(p) >= 1)
            continue;
        return p;
    }
}
Vec3 reflect(Vec3 vec1, Vec3 vec2)
{
    return vec3_sub(vec1, vec3_mul_val(vec2, (vec3_dot(vec1, vec2) * 2.0)));
}

Vec3 refract(Vec3 vec1, Vec3 vec2, double ni_over_nt)
{
    double cos_theta = fmin(vec3_dot(vec3_inv(vec1), vec2), 1.0);
    Vec3 r_out_perp = vec3_mul_val(vec3_add(vec1, vec3_mul_val(vec2, cos_theta)), ni_over_nt);
    Vec3 r_out_parl = vec3_mul_val(vec2, -fast_sqrt(fabs(1.0 - vec3_squared_length(r_out_perp))));

    return vec3_add(r_out_perp, r_out_parl);
}

Vec3 random_vec3_in_hemisphere(Vec3 normal)
{
    Vec3 in_unit = random_vec3_unit();

    if (vec3_dot(in_unit, normal) > 0.0)
    {
        return in_unit;
    }
    return vec3_inv(in_unit);
}

bool is_vec3_near_zero(Vec3 vec)
{
    const double precision = 1e-8;

    return (fabs(vec.x) < precision) && (fabs(vec.y) < precision) && (fabs(vec.z) < precision);
}

Vec3 vec_from_color(Color col)
{
    return vec3_create(col.r, col.g, col.b);
}

Color vec_to_color(Vec3 from)
{
    return color_create(
        from.x,
        from.y,
        from.z);
}
