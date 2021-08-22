#include "shapes.h"
#include <stdlib.h>
bool hit_sphere_object_callback(Ray ray, float t_min, float t_max, HitRecord *record, const Sphere *self)
{
    /* the vec from the ray to the sphere (that must be under sphere.radius to be in collision) */
    Vec3 oc = vec3_sub(ray.origin, self->pos);

    float a = vec3_dot(ray.direction, ray.direction); /* ray length^2*/
    float b = vec3_dot(oc, ray.direction);
    float c = vec3_dot(oc, oc) - (self->radius * self->radius);

    float discriminant = b * b - a * c; /* ^ = b^2 - 4ac */
    float temp;
    if (discriminant > 0)
    {
        temp = (-b - sqrt(b * b - a * c)) / a;
        if (temp < t_max && temp > t_min)
        {
            record->t = temp;
            record->pos = ray_point_param(ray, temp);
            record->normal = vec3_div_val(vec3_sub(record->pos, self->pos), self->radius);
            return true;
        }
        temp = (-b + sqrt(b * b - a * c)) / a;

        if (temp < t_max && temp > t_min)
        {
            record->t = temp;
            record->pos = ray_point_param(ray, temp);
            record->normal = vec3_div_val(vec3_sub(record->pos, self->pos), self->radius);
            return true;
        }
    }
    return false;
}

Sphere *sphere_create(float radius, Vec3 pos)
{
    Sphere *sphere = malloc(sizeof(Sphere));
    sphere->radius = radius;
    sphere->pos = pos;
    return sphere;
}
