#include "perlin.h"

static void permute(int *point, int id)
{
    size_t i;
    int target;
    int tmp;

    for (i = id - 1; i > 0; i--)
    {
        target = (int)(random_rt_float() * i);
        tmp = point[i];

        point[i] = point[target];
        point[target] = tmp;
    }
}
static int *create_perm(size_t point_count)
{
    int *perm = malloc(sizeof(int) * point_count);
    size_t i = 0;
    for (i = 0; i < point_count; i++)
    {
        perm[i] = i;
    }

    permute(perm, point_count);
    return perm;
}

PerlinNoise noise_create(void)
{
    PerlinNoise result;
    size_t i = 0;
    result.point_count = 256;
    result.perm_x = create_perm(result.point_count);
    result.perm_y = create_perm(result.point_count);
    result.perm_z = create_perm(result.point_count);
    result.ranvec = malloc(sizeof(Vec3) * result.point_count);

    result.ran_float = malloc(sizeof(rt_float) * result.point_count);
    for (i = 0; i < result.point_count; i++)
    {
        result.ran_float[i] = random_rt_float();
    }
    for (i = 0; i < result.point_count; i++)
    {
        result.ranvec[i] = (vec3_unit(vec3_create(random_rt_float() * 2 - 1, random_rt_float() * 2 - 1, random_rt_float() * 2 - 1)));
    }
    return result;
}

rt_float trilinear_interp(rt_float c[2][2][2], rt_float u, rt_float v, rt_float w);
rt_float trilinear_interp(rt_float c[2][2][2], rt_float u, rt_float v, rt_float w)
{
    rt_float accum = 0.0;
    rt_float i, j, k;
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 2; j++)
        {
            for (k = 0; k < 2; k++)
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
    int i, j, k;
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 2; j++)
        {
            for (k = 0; k < 2; k++)
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
    rt_float u, v, w;
    int i, j, k;
    Vec3 c[2][2][2];
    int di, dj, dk;

    u = point->x - floor(point->x);
    v = point->y - floor(point->y);
    w = point->z - floor(point->z);

    u = u * u * (3 - 2 * u);
    v = v * v * (3 - 2 * v);
    w = w * w * (3 - 2 * w);

    i = floor(point->x);
    j = floor(point->y);
    k = floor(point->z);

    for (di = 0; di < 2; di++)
    {
        for (dj = 0; dj < 2; dj++)
        {
            for (dk = 0; dk < 2; dk++)
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
    int i;
    (void)depth;

    for (i = 0; i < depth; i++)
    {
        accum += weight * noise(&temp_pos, self);

        weight *= 0.5;
        temp_pos = vec3_mul_val(temp_pos, 2);
    }

    return fabs(accum);
}
