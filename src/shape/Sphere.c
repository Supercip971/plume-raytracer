#include <stdlib.h>
#include "../utils.h"
#include "Sphere.h"

/* this is so bad there is too much arg >:()*/
static bool hit_sphere_object_callback2(rt_float discriminant, rt_float a, rt_float b, rt_float t_min, rt_float t_max, HitRecord *record)
{
    rt_float t;
    rt_float discriminent_root = fast_sqrt(discriminant);

    t = (-b - discriminent_root) / a;

    if (!(t < t_max && t > t_min))
    {
        t = (-b + discriminent_root) / a;

        if (!(t < t_max && t > t_min)) /* if it is still not in range just return false */
        {
            return false;
        }
    }

    record->t = t;

    return true;
}
bool sphere_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const Sphere *self)
{
    (void)time_start;
    (void)time_end;
    *output = self->self_bounding_box;

    return true;
}
FLATTEN bool hit_sphere_object_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const Sphere *self)
{

    /* the vec from the ray to the sphere (that must be under sphere.radius to be in collision) */
    Vec3 oc = vec3_sub(ray.origin, self->pos);
    Vec3 outward = {0};

    rt_float a = vec3_squared_length(ray.direction); /* ray length^2*/
    rt_float b = vec3_dot(oc, ray.direction);
    rt_float c = vec3_squared_length(oc) - (self->radius_squared);

    rt_float discriminant = b * b - a * c; /* ^ = b^2 - 4ac */

    if (discriminant > 0.f && a != 0)
    {
        if (hit_sphere_object_callback2(discriminant, a, b, t_min, t_max, record))
        {
            record->pos = ray_point_param(ray, record->t);
            outward = vec3_mul_val(vec3_sub(record->pos, self->pos), self->radius_inv);

            set_face_normal(&ray, outward, record);
            record->material = self->self_material;
            get_sphere_uv(&outward, &record->u, &record->v);
            return true;
        }
    }
    return false;
}

Object sphere_create(rt_float radius, Vec3 pos, Material sphere_material)
{
    Object result;
    Sphere *sphere = malloc(sizeof(Sphere));
    sphere->self_material = sphere_material;
    sphere->radius = radius;
    sphere->pos = pos;
    sphere->self_bounding_box = aabb_create(vec3_sub(pos, vec3_create(radius, radius, radius)), vec3_add(pos, vec3_create(radius, radius, radius)));

    sphere->radius_inv = 1 / radius;
    sphere->radius_squared = radius * radius;
    result.data = sphere;
    result.type = SHAPE_SPHERE;
    result.is_leaf = true;

    return result;
}
