#include "triangle.h"
#include "hittable.h"
#include "math/aabb.h"
#include "math/ray.h"
#include "math/vec3.h"
#include "utils/utils.h"

Object triangle_create(Vec3 posa, Vec3 posb, Vec3 posc)
{
    Triangle *self = malloc(sizeof(Triangle));
    self->posa = posa;
    self->posb = posb;
    self->posc = posc;
    self->uva = (Vec2){0, 0};
    self->uvb = (Vec2){0, 0};
    self->uvc = (Vec2){0, 0};

    return (Object){
        .data = self,
        .type = SHAPE_TRIANGLE,
    };
}

bool hit_triangle_object_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const Triangle *self)
{
    Vec3 v0 = self->posa;
    Vec3 v1 = self->posb;
    Vec3 v2 = self->posc;

    Vec3 edge_1 = vec3_sub(v1, v0);
    Vec3 edge_2 = vec3_sub(v2, v0);
    Vec3 h = vec3_cross(ray.direction, edge_2);
    float a = vec3_dot(edge_1, h);
    if (rt_abs(a) < 0.0001)
    {
        return false;
    }
    float inv_a = 1.0 / a;
    Vec3 s = vec3_sub(ray.origin, v0);
    float u = inv_a * vec3_dot(s, h);
    if (u < 0.0 || u > 1.0)
    {
        return false;
    }
    Vec3 q = vec3_cross(s, edge_1);
    float v = inv_a * vec3_dot(ray.direction, q);
    if (v < 0 || u + v > 1.0)
    {
        return false;
    }

    float t = inv_a * vec3_dot(edge_2, q);

    if (t < t_min || t > t_max)
    {
        return false;
    }
    Vec2 final =
        vec2_add(vec2_add(vec2_mul_val(self->uva, (1 - u - v)),
                          vec2_mul_val(self->uvb, (u))),
                 vec2_mul_val(self->uvc, (v)));

    *record = (HitRecord){
        .u = final.x,
        .v = final.y,
        .t = t,
        .pos = ray_point_param(ray, t),

    };
    set_face_normal(&ray, vec3_unit(vec3_cross(edge_1, edge_2)), record);
    return true;
}
bool triangle_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const Triangle *self)
{
    (void)time_start;
    (void)time_end;
    *output = aabb_create(vec3_min(vec3_min(self->posa, self->posb), self->posc), vec3_max(vec3_max(self->posa, self->posb), self->posc));
    return true;
}

rt_float triangle_pdf_value(Vec3 origin, Vec3 direction, const Triangle *self)
{
    HitRecord rec = {};
    Ray r = {.origin = origin, .direction = direction, .time = 0};
    ray_dir_init(&r);
    if (!hit_triangle_object_callback(r, 0.001, 100000, &rec, self))
    {
        return 0;
    }

    Vec3 v0 = self->posa;
    Vec3 v1 = self->posb;
    Vec3 v2 = self->posc;

    Vec3 edge_1 = vec3_sub(v1, v0);
    Vec3 edge_2 = vec3_sub(v2, v0);

    float area = vec3_invlength(vec3_cross(edge_1, edge_2));

    float cosine = rt_abs(vec3_dot((direction), rec.normal)) * vec3_invlength(direction);

    float distance = rec.t * rec.t * vec3_squared_length(direction);
    float pdf = (distance * area) / (cosine);

    return pdf;
}
Vec3 triangle_random(Vec3 origin, const Triangle *self)
{
    Vec3 v0 = self->posa;
    Vec3 v1 = self->posb;
    Vec3 v2 = self->posc;

    Vec3 edge_1 = vec3_sub(v1, v0);
    Vec3 edge_2 = vec3_sub(v2, v0);
    float u = random_rt_float();
    float v = random_rt_float();
    if (u + v > 1.f)
    {
        u = 1.f - u;
        v = 1.f - v;
    }

    Vec3 barycentric_coordinates = vec3_add(vec3_mul_val(edge_1, u), vec3_add(vec3_mul_val(edge_2, v), v0));

    return (vec3_sub(barycentric_coordinates, origin));
}