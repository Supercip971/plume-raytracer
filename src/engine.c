#include "engine.h"
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "camera.h"
#include "ray.h"
#include "shapes.h"
#include "utils.h"
#include "vec3.h"

/* https://www.realtimerendering.com/raytracing/Ray%20Tracing%20in%20a%20Weekend.pdf */

static Vec3 ccol(Ray from, float time)
{
    HitRecord record = {0};
    float t;
    Vec3 unit_direction;
    (void)time;

    if (hit_call_all_object(from, 0.0, 100000000, &record))
    {
        return vec3_mul_val(vec3_create(record.normal.x + 1, record.normal.y + 1, record.normal.z + 1), 0.5);
    }
    else
    {
        unit_direction = vec3_unit(from.direction);
        t = 0.5 * (unit_direction.y + 1);
        return vec3_add(vec3_mul_val(vec3_create(1, 1, 1), 1 - t), vec3_mul_val(vec3_create(0.5, 0.7, 1), t));
    }
}

void render_update(Color *framebuffer, size_t width, size_t height)
{
    static float t = 0.0f;
    static const int sample_count = 4;
    static const int sample_step = (int)sample_count / 2;
    Camera camera = get_camera_default();
    Vec3 current_color;
    Ray r;
    float x, y;
    int sample;

    t += 1.f;
    for (x = 0; x < width; x++)
    {
        for (y = 0; y < height; y++)
        {
            Vec3 col = {0};
            for (sample = 0; sample < sample_count; sample++)
            {

                float u = (x + ((float)(sample % sample_step) / sample_count)) / (float)width;
                float v = (y + ((float)(sample / sample_step) / sample_count)) / (float)height;
                r = get_camera_ray(&camera, u, v);
                current_color = (ccol(r, t));
                col = vec3_add(col, current_color);
            }

            framebuffer[(int)x + (int)y * width] = vec_to_color(vec3_div_val(col, sample_count));
        }
    }
}

void render_init(void)
{
    add_hitable_object((HitCallback)hit_sphere_object_callback, sphere_create(0.5, vec3_create(0, 0, -1)));
    add_hitable_object((HitCallback)hit_sphere_object_callback, sphere_create(100, vec3_create(0, -100.5, -1)));
}

void render_deinit(void)
{
    hit_destroy_all_objects();
}
