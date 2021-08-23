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

Camera get_camera_default(void);
