#include "shapes.h"
#include <stdlib.h>
bool hit_sphere_object_callback(Ray ray, double t_min, double t_max, HitRecord *record, const Sphere *self)
{
    /* the vec from the ray to the sphere (that must be under sphere.radius to be in collision) */
    Vec3 oc = vec3_sub(ray.origin, self->pos);

    double a = vec3_squared_length(ray.direction); /* ray length^2*/
    double b = vec3_dot(oc, ray.direction);
    double c = vec3_dot(oc, oc) - (self->radius * self->radius);

    double discriminant = b * b - a * c; /* ^ = b^2 - 4ac */
    double discriminent_root = sqrt(discriminant);
    double temp;
    if (discriminant > 0)
    {
        temp = (-b - discriminent_root) / a;
        if (temp < t_max && temp > t_min)
        {
            record->t = temp;
            record->pos = ray_point_param(ray, temp);
            record->normal = vec3_div_val(vec3_sub(record->pos, self->pos), self->radius);
            return true;
        }

        temp = (-b + discriminent_root) / a;

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

Sphere *sphere_create(double radius, Vec3 pos)
{
    Sphere *sphere = malloc(sizeof(Sphere));
    sphere->radius = radius;
    sphere->pos = pos;
    return sphere;
}
