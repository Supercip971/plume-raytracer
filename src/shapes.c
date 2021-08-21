#include "shapes.h"

bool ray_sphere_collide(Ray ray, Sphere sphere)
{
    Vec3 oc = vec3_sub(ray.origin, sphere.pos);
    float a = vec3_dot(ray.direction, ray.direction);
    float b = 2 * vec3_dot(oc, ray.direction);
    float c = vec3_dot(oc, oc) - (sphere.radius * sphere.radius);
    float discriminant = b * b - 4 * a * c;
    return discriminant > 0;
}
