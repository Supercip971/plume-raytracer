#include "camera.h"
#include <stdio.h>
#include "utils.h"
Ray get_camera_ray(const Camera *camera, rt_float u, rt_float v)
{

    Vec3 rd = vec3_mul_val(random_vec3_unit_in_disk(), camera->lens_radius);
    Vec3 offset = vec3_add(
        vec3_mul_val(camera->u, rd.x),
        vec3_mul_val(camera->v, rd.y));

    Ray result = {
        .origin = vec3_add(camera->origin, offset),
        .direction = vec3_sub(
            vec3_add(
                vec3_add(
                    camera->low_left, vec3_mul_val(camera->horizontal, u)),
                vec3_mul_val(camera->vertical, v)),
            (result.origin)),
    };

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
    rt_float theta = config.vfov * M_PI / 180;
    rt_float half_height = tan(theta / 2);
    rt_float half_width = config.aspect * half_height;

    Vec3 w = vec3_unit(vec3_sub(config.position, config.lookat));
    Vec3 u = vec3_unit(vec3_cross(config.up, w));
    Vec3 v = vec3_cross(w, u);

    return (Camera){
        .time_end = config.time_end,
        .time_start = config.time_start,
        .origin = config.position,
        .horizontal = vec3_mul_val(u, 2 * half_width * config.focus_distance),
        .vertical = vec3_mul_val(v, 2 * half_height * config.focus_distance),
        .low_left =    vec3_sub(
        vec3_sub(
            vec3_sub(config.position,
                     vec3_mul_val(u, half_width * config.focus_distance)),
            vec3_mul_val(v, half_height * config.focus_distance)),
        vec3_mul_val(w, config.focus_distance)),
    .u = u,
        .v = v,
        .w = w,
        .lens_radius = config.aperture / 2,
        .moving_obj = config.moving_obj,

    };
}
