#include "camera.h"

Ray get_camera_ray(const Camera *camera, double u, double v)
{
    Ray result;
    result.origin = camera->origin;
    result.direction = vec3_add(
        vec3_add(vec3_mul_val(camera->horizontal, u),
                 vec3_sub(vec3_mul_val(camera->vertical, 1-v), camera->origin)),
        camera->low_left);
    return result;
}

Camera create_camera(Vec3 position, Vec3 lookat, Vec3 up, float vfov, float aspect)
{
    Camera result;
    Vec3 u, v, w;

    float theta = vfov * M_PI / 180;
    float half_height = tan(theta / 2);
    float half_width = aspect * half_height;

    w = vec3_unit(vec3_sub(position, lookat));
    u = vec3_unit(vec3_cross(up, w));
    v = vec3_cross(w, u);

    result.origin = position;
    result.horizontal = vec3_mul_val(u, 2 * half_width);
    result.vertical = vec3_mul_val(v, 2 * half_height);
    /*result.low_left = vec3_sub(position, vec3_add(vec3_add(vec3_mul_val(u, half_width), vec3_mul_val(v, half_height)), w));*/
    result.low_left = vec3_sub(vec3_sub(vec3_sub(position, vec3_mul_val(u, half_width)), vec3_mul_val(v, half_height)), w);

    /*result.low_left = vec3_sub(position, vec3_add(vec3_mul_val(result.horizontal, 0.5), vec3_add(vec3_mul_val(result.vertical, 0.5), w)));*/
    return result;
}
