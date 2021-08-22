#include "engine.h"
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include "ray.h"
#include "shapes.h"
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
    Vec3 low_left = vec3_create(-3, -2, -2);
    Vec3 horizontal = vec3_create(6, 0, 0);
    Vec3 vertical = vec3_create(0, 4, 0);
    Vec3 origin = vec3_create(0, 0, 0);
    Color col;
    Ray r;

    size_t x;
    size_t y;
    t += 1.f;
    for (x = 0; x < width; x++)
    {
        for (y = 0; y < height; y++)
        {
            float u = (float)x / (float)width;
            float v = (float)y / (float)height;
            r.origin = origin;
            r.direction = vec3_add(
                vec3_add(vec3_mul_val(horizontal, u),
                         vec3_mul_val(vertical, 1 - v)),
                low_left);

            col = vec_to_color(ccol(r, t));
            framebuffer[x + y * width] = col;
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
