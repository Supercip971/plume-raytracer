#include "camera.h"

Ray get_camera_ray(const Camera *camera, double u, double v)
{
    Ray result;
    result.origin = camera->origin;
    result.direction = vec3_add(
        vec3_add(vec3_mul_val(camera->horizontal, u),
                 vec3_sub(vec3_mul_val(camera->vertical, 1 - v), camera->origin)),
        camera->low_left);
    return result;
}

Camera get_camera_default(void)
{
    Camera result;
    result.low_left = vec3_create(-8, -4.5, -4.5);
    result.horizontal = vec3_create(16, 0, 0);
    result.vertical = vec3_create(0, 9, 0);
    result.origin = vec3_create(0, 0, 0);
    return result;
}
