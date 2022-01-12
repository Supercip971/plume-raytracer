#include "aa_rec.h"
#include <stdlib.h>
#include "../utils.h"

/* TODO: make this code better because it's shit for now */

rt_float aa_xyrect_pdf_value(Vec3 origin, Vec3 direction, XYrec *self)
{
    HitRecord rec = {};
    Ray r = {.origin = origin, .direction = direction, .time = 0};
    rt_float area;
    rt_float distance_squared;
    rt_float cosine;

    if (!hit_aaxyrect_callback(r, 0.001, 10000000, &rec, self))
    {
        return 0;
    }

    area = (self->x_1 - self->x_0) * (self->y_1 - self->y_0);

    distance_squared = rec.t * rec.t * vec3_squared_length(direction);

    cosine = rt_abs(vec3_dot(direction, rec.normal) / vec3_length(direction));

    return distance_squared / (cosine * area);
}

rt_float aa_xzrect_pdf_value(Vec3 origin, Vec3 direction, XZrec *self)
{
    HitRecord rec = {};
    Ray r = {.origin = origin, .direction = direction, .time = 0};
    rt_float area;
    rt_float distance_squared;
    rt_float cosine;

    if (!hit_aaxzrect_callback(r, 0.001, 1000000, &rec, self))
    {
        return 0;
    }

    area = (self->x_1 - self->x_0) * (self->z_1 - self->z_0);

    distance_squared = rec.t * rec.t * vec3_squared_length(direction);

    cosine = rt_abs(vec3_dot(direction, rec.normal) / vec3_length(direction));

    return distance_squared / (cosine * area);
}
rt_float aa_yzrect_pdf_value(Vec3 origin, Vec3 direction, YZrec *self)
{
    HitRecord rec = {};
    Ray r = {.origin = origin, .direction = direction, .time = 0};
    rt_float area;
    rt_float distance_squared;
    rt_float cosine;

    if (!hit_aayzrect_callback(r, 0.001, 100000, &rec, self))
    {
        return 0;
    }

    area = (self->y_1 - self->y_0) * (self->z_1 - self->z_0);

    distance_squared = rec.t * rec.t * vec3_squared_length(direction);

    cosine = rt_abs(vec3_dot(direction, rec.normal) / vec3_length(direction));

    return distance_squared / (cosine * area);
}

Vec3 aa_xyrect_pdf_random(Vec3 origin, XYrec *self)
{
    Vec3 direction = vec3_create(random_rt_range(self->x_0, self->x_1), random_rt_range(self->y_0, self->y_1), self->k);

    return vec3_sub(direction, origin);
}

Vec3 aa_xzrect_pdf_random(Vec3 origin, XZrec *self)
{
    Vec3 direction = vec3_create(random_rt_range(self->x_0, self->x_1), self->k, random_rt_range(self->z_0, self->z_1));

    return vec3_sub(direction, origin);
}

Vec3 aa_yzrect_pdf_random(Vec3 origin, YZrec *self)
{
    Vec3 direction = vec3_create(self->k, random_rt_range(self->y_0, self->y_1), random_rt_range(self->z_0, self->z_1));

    return vec3_sub(direction, origin);
}

FLATTEN bool hit_aaxyrect_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const XYrec *self)
{
    rt_float t;
    rt_float x, y;
    Vec3 outward;

    t = (self->k - ray.origin.z) / (ray.direction.z + 0.0001);

    if (t < t_min || t > t_max)
    {
        return false;
    }

    x = ray.origin.x + t * ray.direction.x;
    y = ray.origin.y + t * ray.direction.y;

    if (x < self->x_0 || x > self->x_1 || y < self->y_0 || y > self->y_1)
    {
        return false;
    }

    record->u = (x - self->x_0) / (self->x_1 - self->x_0);
    record->v = (y - self->y_0) / (self->y_1 - self->y_0);
    record->t = t;

    outward = vec3_create(0, 0, 1);
    set_face_normal(&ray, outward, record);

    record->material = self->self_material;
    record->pos = ray_point_param(ray, t);

    return true;
}

FLATTEN bool aaxyrect_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const XYrec *self)
{
    (void)time_start;
    (void)time_end;
    *output = self->self_box;

    return true;
}

Object aaxyrect_create(rt_float x_0, rt_float x_1, rt_float y_0, rt_float y_1, rt_float k, Material rect_material)
{
    Object result;
    XYrec *rect = malloc(sizeof(XYrec));
    rect->self_material = rect_material;
    rect->self_box = aabb_create(vec3_create(x_0, y_0, k - 0.0001), vec3_create(x_1, y_1, k + 0.0001));

    rect->k = k;

    rect->x_0 = x_0;
    rect->x_1 = x_1;
    rect->y_0 = y_0;
    rect->y_1 = y_1;

    result.data = rect;
    result.type = SHAPE_AAREC_XY;
    result.is_leaf = true;

    return result;
}

/* xz rect */

FLATTEN bool hit_aaxzrect_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const XZrec *self)
{
    rt_float t;
    rt_float z, x;
    Vec3 outward;

    t = (self->k - ray.origin.y) / (ray.direction.y + 0.0001);

    if (t < t_min || t > t_max)
    {
        return false;
    }

    z = ray.origin.z + t * ray.direction.z;
    x = ray.origin.x + t * ray.direction.x;

    if (z < self->z_0 || z > self->z_1 || x < self->x_0 || x > self->x_1)
    {
        return false;
    }

    record->u = (x - self->x_0) / (self->x_1 - self->x_0);
    record->v = (z - self->z_0) / (self->z_1 - self->z_0);
    record->t = t;

    outward = vec3_create(0, 1, 0);
    set_face_normal(&ray, outward, record);

    record->material = self->self_material;
    record->pos = ray_point_param(ray, t);

    return true;
}

FLATTEN bool aaxzrect_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const XZrec *self)
{
    (void)time_start;
    (void)time_end;
    *output = self->self_box;

    return true;
}
Object aaxzrect_create(rt_float x_0, rt_float x_1, rt_float z_0, rt_float z_1, rt_float k, Material rect_material)
{

    Object result;
    XZrec *rect = malloc(sizeof(XZrec));
    rect->self_material = rect_material;
    rect->self_box = aabb_create(vec3_create(x_0, k - 0.0001, z_0), vec3_create(x_1, k + 0.0001, z_1));

    rect->k = k;

    rect->z_0 = z_0;
    rect->z_1 = z_1;
    rect->x_0 = x_0;
    rect->x_1 = x_1;
    result.type = SHAPE_AAREC_XZ;
    result.data = rect;
    result.is_leaf = true;

    return result;
}
/* yz rect */

FLATTEN bool hit_aayzrect_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const YZrec *self)
{
    rt_float t;
    rt_float z, y;
    Vec3 outward;

    t = (self->k - ray.origin.x) / (ray.direction.x + 0.0001);

    if (t < t_min || t > t_max)
    {
        return false;
    }

    z = ray.origin.z + t * ray.direction.z;
    y = ray.origin.y + t * ray.direction.y;

    if (z < self->z_0 || z > self->z_1 || y < self->y_0 || y > self->y_1)
    {
        return false;
    }

    record->u = (y - self->y_0) / (self->y_1 - self->y_0);
    record->v = (z - self->z_0) / (self->z_1 - self->z_0);
    record->t = t;

    outward = vec3_create(1, 0, 0);
    set_face_normal(&ray, outward, record);

    record->material = self->self_material;
    record->pos = ray_point_param(ray, t);

    return true;
}

FLATTEN bool aayzrect_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const YZrec *self)
{
    (void)time_start;
    (void)time_end;
    *output = self->self_box;

    return true;
}
Object aayzrect_create(rt_float y_0, rt_float y_1, rt_float z_0, rt_float z_1, rt_float k, Material rect_material)
{

    Object result;
    YZrec *rect = malloc(sizeof(YZrec));
    rect->self_material = rect_material;
    rect->self_box = aabb_create(vec3_create(k - 0.0001, y_0, z_0), vec3_create(k + 0.0001, y_1, z_1));

    rect->k = k;

    rect->z_0 = z_0;
    rect->z_1 = z_1;
    rect->y_0 = y_0;
    rect->y_1 = y_1;
    result.type = SHAPE_AAREC_YZ;
    result.data = rect;
    result.is_leaf = true;

    return result;
}
