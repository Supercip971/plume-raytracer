#include "camera.h"
#include <stdio.h>
#include "utils.h"
Ray get_camera_ray(const Camera *camera, rt_float u, rt_float v)
{
    Ray result;
    Vec3 rd, offset;

    rd = vec3_mul_val(random_vec3_unit_in_disk(), camera->lens_radius);
    offset = vec3_add(
        vec3_mul_val(camera->u, rd.x),
        vec3_mul_val(camera->v, rd.y));

    result.origin = vec3_add(camera->origin, offset);
    result.direction = vec3_sub(
        vec3_add(
            vec3_add(
                camera->low_left, vec3_mul_val(camera->horizontal, u)),
            vec3_mul_val(camera->vertical, v)),
        (result.origin));

    if (camera->moving_obj)
    {
        result.time = random_rt_float() * (camera->time_end - camera->time_start) + camera->time_start;
    }
    else
    {
        result.time = 0;
    }

    return ray_dir_init(&result);
}

Camera create_camera(CameraConfig config)
{
    Camera result;
    Vec3 u, v, w;

    rt_float theta = config.vfov * M_PI / 180;
    rt_float half_height = tan(theta / 2);
    rt_float half_width = config.aspect * half_height;

    result.time_end = config.time_end;
    result.time_start = config.time_start;

    w = vec3_unit(vec3_sub(config.position, config.lookat));
    u = vec3_unit(vec3_cross(config.up, w));
    v = vec3_cross(w, u);

    result.lens_radius = config.aperture / 2;

    result.origin = config.position;
    result.horizontal = vec3_mul_val(u, 2 * half_width * config.focus_distance);
    result.vertical = vec3_mul_val(v, 2 * half_height * config.focus_distance);
    /*result.low_left = vec3_sub(position, vec3_add(vec3_add(vec3_mul_val(u, half_width), vec3_mul_val(v, half_height)), w));*/
    result.low_left = vec3_sub(
        vec3_sub(
            vec3_sub(config.position,
                     vec3_mul_val(u, half_width * config.focus_distance)),
            vec3_mul_val(v, half_height * config.focus_distance)),
        vec3_mul_val(w, config.focus_distance));

    result.u = u;
    result.v = v;
    result.w = w;

    result.moving_obj = config.moving_obj;
    return result;
}
