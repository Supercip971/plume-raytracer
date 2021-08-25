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
    double lens_radius;

} Camera;

struct camera_config
{
    Vec3 position;
    Vec3 lookat;
    Vec3 up;
    double vfov;
    double aspect;
    double aperture;
    double focus_distance;
};

Ray get_camera_ray(const Camera *camera, double u, double v);

Camera create_camera(struct camera_config config);
