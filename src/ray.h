#ifndef RAY_H
#define RAY_H
#include "config.h"
#include "vec3.h"

typedef struct ray
{
    Vec3 origin;
    Vec3 direction;
    rt_float time;
    Vec3 inv_dir;
} Ray;

static inline Vec3 ray_point_param(Ray self, rt_float t)
{
    return vec3_add(self.origin, vec3_mul_val(self.direction, t));
}

static inline Ray ray_dir_init(Ray* self)
{
       self->inv_dir = vec3_div(vec3_create(1, 1, 1), self->direction);
       return *self;
}

#endif
