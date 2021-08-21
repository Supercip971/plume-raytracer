#include "engine.h"
#include "ray.h"
#include "shapes.h"
#include "vec3.h"

/* https://www.realtimerendering.com/raytracing/Ray%20Tracing%20in%20a%20Weekend.pdf */

static Vec3 ccol(Ray from, float time)
{

    Vec3 u_direction = vec3_unit(from.direction);
    Vec3 sphere_pos = vec3_create(0, sin(time / 100), -1);
    float t = 0.5 * (u_direction.y + 1.0);
    Sphere sphere;
    float v = 0;
    Vec3 color;
    sphere.radius = 0.5;
    sphere.pos = sphere_pos;
    v = ray_sphere_collide(from, sphere);

    if (v > 0)
    {
        color = vec3_sub(vec3_unit(ray_point_param(from, v)), vec3_create(0, 0, 1));

        color = vec3_mul_val(vec3_create(color.x + 1, color.y + 1, color.z + 1), 0.5);
    }
    else
    {

        color = vec3_add(vec3_mul_val(vec3_create(1, 1, 1), 1 - t),
                         vec3_mul_val(vec3_create(0.5, 0.7, 1), t));
    }
    return color;
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
