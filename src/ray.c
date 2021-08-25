#include "ray.h"

Vec3 ray_point_param(Ray self, double t)
{
    return vec3_add(self.origin, vec3_mul_val(self.direction, t));
}
