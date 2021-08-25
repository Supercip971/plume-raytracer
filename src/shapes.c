#include "shapes.h"
#include <stdlib.h>

/* this is so bad there is too much arg >:()*/
static bool hit_sphere_object_callback2(double discriminant, double a, double b, Ray ray, double t_min, double t_max, HitRecord *record, const Sphere *self)
{
    double temp;
    double discriminent_root = sqrt(discriminant);

    temp = (-b - discriminent_root) / a;

    if (temp < t_max && temp > t_min)
    {
        record->t = temp;
        record->pos = ray_point_param(ray, temp);
        set_face_normal(&ray, vec3_div_val(vec3_sub(record->pos, self->pos), self->radius), record);
        return true;
    }

    temp = (-b + discriminent_root) / a;

    if (temp < t_max && temp > t_min)
    {
        record->t = temp;
        record->pos = ray_point_param(ray, temp);
        set_face_normal(&ray, vec3_div_val(vec3_sub(record->pos, self->pos), self->radius), record);
        return true;
    }
    return false;
}
bool hit_sphere_object_callback(Ray ray, double t_min, double t_max, HitRecord *record, const Sphere *self)
{
    /* the vec from the ray to the sphere (that must be under sphere.radius to be in collision) */
    Vec3 oc = vec3_sub(ray.origin, self->pos);

    double a = vec3_squared_length(ray.direction); /* ray length^2*/
    double b = vec3_dot(oc, ray.direction);
    double c = vec3_squared_length(oc) - (self->radius * self->radius);

    double discriminant = b * b - a * c; /* ^ = b^2 - 4ac */
    if (discriminant > 0.f && a != 0)
    {
        return hit_sphere_object_callback2(discriminant, a, b, ray, t_min, t_max, record, self);
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
