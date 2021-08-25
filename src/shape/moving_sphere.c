#include <stdlib.h>
#include "../shapes.h"
#include "../utils.h"
#include "moving_sphere.h"

static Vec3 get_sphere_pos(const MovingSphere *self, rt_float time)
{
    rt_float val = (time - self->time_start) / (self->time_end - self->time_start);

    return vec3_add(self->pos_start, vec3_mul_val(vec3_sub(self->pos_end, self->pos_start), val));
}
/* this is so bad there is too much arg >:()*/
static bool hit_moving_sphere_object_callback2(rt_float discriminant, rt_float a, rt_float b, rt_float t_min, rt_float t_max, HitRecord *record)
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
bool hit_moving_sphere_object_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const MovingSphere *self)
{
    /* the vec from the ray to the sphere (that must be under sphere.radius to be in collision) */
    Vec3 origin = get_sphere_pos(self, ray.time);
    Vec3 oc = vec3_sub(ray.origin, origin);

    rt_float a = vec3_squared_length(ray.direction); /* ray length^2*/
    rt_float b = vec3_dot(oc, ray.direction);
    rt_float c = vec3_squared_length(oc) - (self->radius * self->radius);

    rt_float discriminant = b * b - a * c; /* ^ = b^2 - 4ac */

    if (discriminant > 0.f && a != 0)
    {
        if (hit_moving_sphere_object_callback2(discriminant, a, b, t_min, t_max, record))
        {
            record->pos = ray_point_param(ray, record->t);
            set_face_normal(&ray, vec3_div_val(vec3_sub(record->pos, origin), self->radius), record);
            return true;
        }
    }
    return false;
}
Object moving_sphere_create(rt_float radius, rt_float time_start, rt_float time_end, Vec3 pos_start, Vec3 pos_end)
{
    Object result;
    MovingSphere *sphere = malloc(sizeof(MovingSphere));
    sphere->radius = radius;
    sphere->time_end = time_end;
    sphere->time_start = time_start;
    sphere->pos_start = pos_start;
    sphere->pos_end = pos_end;

    result.collide = (ObjectCallback)hit_moving_sphere_object_callback;
    result.data = sphere;

    return result;
}