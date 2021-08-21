#include "engine.h"
#include "ray.h"
#include "shapes.h"
#include "vec3.h"

// https://www.realtimerendering.com/raytracing/Ray%20Tracing%20in%20a%20Weekend.pdf

Vec3 color(Ray from)
{

    Vec3 u_direction = vec3_unit(from.direction);
    Vec3 sphere_pos = vec3_create(0, 0, -1);
    float t = 0.5 * (u_direction.y + 1.0);

    if (ray_sphere_collide(from, (Sphere){.pos = sphere_pos, .radius = 0.5}))
    {
        return vec3_create((vec3_dot(from.origin, sphere_pos) / 2), 0, 0);
    }

    return vec3_add(vec3_mul_val(vec3_create(1, 1, 1), 1 - t),
                    vec3_mul_val(vec3_create(0.5, 0.7, 1), t));
}

void render_update(Color *framebuffer, size_t width, size_t height)
{
    Vec3 low_left = vec3_create(-8, -4.5, -4.5);
    Vec3 horizontal = vec3_create(16, 0, 0);
    Vec3 vertical = vec3_create(0, 9, 0);
    Vec3 origin = vec3_create(0, 0, 0);
    Color col;
    Ray r;

    size_t x;
    size_t y;
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

            col = vec_to_color(color(r));
            framebuffer[x + y * width] = col;
        }
    }
}
