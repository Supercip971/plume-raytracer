#pragma once

#include "ray.h"
#include "vec3.h"

typedef struct
{
    Vec3 low_left;
    Vec3 horizontal;
    Vec3 vertical;
    Vec3 origin;
    Vec3 u, v, w;
    rt_float lens_radius;
    rt_float time_start, time_end;
    bool moving_obj;
} Camera;

struct camera_config
{
    Vec3 position;
    Vec3 lookat;
    Vec3 up;
    rt_float vfov;
    rt_float aspect;
    rt_float aperture;
    rt_float focus_distance;
    rt_float time_start, time_end;
    bool moving_obj;
};

Ray get_camera_ray(const Camera *camera, rt_float u, rt_float v);

Camera create_camera(struct camera_config config);
