#pragma once

#include "ray.h"
#include "vec3.h"

typedef struct
{
    Vec3 low_left;
    Vec3 horizontal;
    Vec3 vertical;
    Vec3 origin;

} Camera;

Ray get_camera_ray(const Camera *camera, double u, double v);

Camera create_camera(Vec3 position, Vec3 lookat, Vec3 up, float vfov, float aspect);
