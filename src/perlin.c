#include "perlin.h"

static void permute(int *point, int id)
{
    for (size_t i = id - 1; i > 0; i--)
    {
        int target = (int)(random_rt_float() * i);
        int tmp = point[i];

        point[i] = point[target];
        point[target] = tmp;
    }
}

static int *create_perm(size_t point_count)
{
    int *perm = malloc(sizeof(int) * point_count);

    for (size_t i = 0; i < point_count; i++)
    {
        perm[i] = i;
    }

    permute(perm, point_count);
    return perm;
}

PerlinNoise noise_create(void)
{
    const int point_count = 256;
    PerlinNoise result = {
        .point_count = point_count,
        .perm_x = create_perm(point_count),
        .perm_y = create_perm(point_count),
        .perm_z = create_perm(point_count),
        .ranvec = malloc(sizeof(Vec3) * point_count),
        .ran_float = malloc(sizeof(rt_float) * point_count),
    };

    for (size_t i = 0; i < result.point_count; i++)
    {
        result.ran_float[i] = random_rt_float();

        result.ranvec[i] = (vec3_unit(vec3_create(random_rt_float() * 2 - 1, random_rt_float() * 2 - 1, random_rt_float() * 2 - 1)));
    }
    return result;
}

rt_float trilinear_interp(rt_float c[2][2][2], rt_float u, rt_float v, rt_float w)
{
    rt_float accum = 0.0;

    for (rt_float i = 0; i < 2; i++)
    {
        for (rt_float j = 0; j < 2; j++)
        {
            for (rt_float k = 0; k < 2; k++)
            {
                accum += (i * u + (1 - i) * (1 - u)) *
                         (j * v + (1 - j) * (1 - v)) *
                         (k * w + (1 - k) * (1 - w)) * c[(int)i][(int)j][(int)k];
            }
        }
    }

    return accum;
}

static rt_float perlin_interp(Vec3 c[2][2][2], rt_float u, rt_float v, rt_float w)
{
    rt_float uu = u * u * (3 - 2 * u);
    rt_float vv = v * v * (3 - 2 * v);
    rt_float ww = w * w * (3 - 2 * w);

    rt_float accum = 0.0;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            for (int k = 0; k < 2; k++)
            {
                Vec3 weight_v = vec3_create(u - i, v - j, w - k);
                accum += (i * uu + (1 - i) * (1 - uu)) *
                         (j * vv + (1 - j) * (1 - vv)) *
                         (k * ww + (1 - k) * (1 - ww)) *
                         vec3_dot(c[i][j][k], weight_v);
            }
        }
    }

    return accum;
}

rt_float noise(const Vec3 *point, const PerlinNoise *self)
{
    Vec3 c[2][2][2];

    rt_float u = point->x - floor(point->x);
    rt_float v = point->y - floor(point->y);
    rt_float w = point->z - floor(point->z);

    u = u * u * (3 - 2 * u);
    v = v * v * (3 - 2 * v);
    w = w * w * (3 - 2 * w);

    int i = floor(point->x);
    int j = floor(point->y);
    int k = floor(point->z);

    for (int di = 0; di < 2; di++)
    {
        for (int dj = 0; dj < 2; dj++)
        {
            for (int dk = 0; dk < 2; dk++)
            {
                c[di][dj][dk] = self->ranvec[self->perm_x[(i + di) & (self->point_count - 1)] ^
                                             self->perm_y[(j + dj) & (self->point_count - 1)] ^
                                             self->perm_z[(k + dk) & (self->point_count - 1)]];
            }
        }
    }

    return perlin_interp(c, u, v, w);
}

rt_float turb(const Vec3 *point, int depth, const PerlinNoise *self)
{
    rt_float accum = 0.0;
    Vec3 temp_pos = *point;
    rt_float weight = 1.0;

    for (int i = 0; i < depth; i++)
    {
        accum += weight * noise(&temp_pos, self);

        weight *= 0.5;
        temp_pos = vec3_mul_val(temp_pos, 2);
    }

    return fabs(accum);
}
