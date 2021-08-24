#include "engine.h"
#include <limits.h>
#include <math.h>
#define __USE_GNU
#include <pthread.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "camera.h"
#include "config.h"
#include "material/dielectric.h"
#include "material/lambertian.h"
#include "material/metal.h"
#include "ray.h"
#include "shapes.h"
#include "utils.h"
#include "vec3.h"

/* https://www.realtimerendering.com/raytracing/Ray%20Tracing%20in%20a%20Weekend.pdf */

const int sample_count_x = RENDER_THREAD;
const int sample_step_x = SCRN_WIDTH / sample_count_x;
const int sample_count_y = RENDER_THREAD;
const int sample_step_y = SCRN_HEIGHT / sample_count_y;
bool stop;

#define double_max (double)((uint64_t)-1)

struct render_thread_args
{
    Color *framebuffer;
    size_t width;
    size_t height;
    size_t s_x;
    size_t s_y;
};
struct render_thread_args args[256];
pthread_t thr[256] = {0};

static Vec3 ccol(Ray from, int depth)
{
    HitRecord record = {0};
    double t;
    Vec3 unit_direction;

    if (hit_call_all_object(from, 0.0, double_max, &record))
    {
        Ray forked_ray = from;
        Vec3 attenuation;

        if (depth < 50 && record.material.material_callback(&from, &record, &attenuation, &forked_ray, record.material.data))
        {
            return vec3_mul(attenuation, ccol(forked_ray, depth + 1));
        }
        else
        {
            return vec3_create(0, 0, 0);
        }
    }
    else
    {
        unit_direction = vec3_unit(from.direction);
        t = 0.5 * (unit_direction.y + 1);
        return vec3_add(vec3_mul_val(vec3_create(1, 1, 1), 1 - t), vec3_mul_val(vec3_create(0.5, 0.7, 1), t));
    }
}

static void render_update_part(Color *framebuffer, size_t width, size_t height, double x_from, double y_from, double x_max, double y_max)
{

    const int sample_count = 16;
    const double sample_inv = (double)1 / sample_count;

    Camera camera = get_camera_default();
    Vec3 current_color;
    Color final_color;
    Ray r;
    size_t x, y;
    int sample;

    (void)height;

    for (x = x_from; x < x_max; x += 1)
    {
        for (y = y_from; y < y_max; y += 1)
        {
            Vec3 col = {0};
            for (sample = 0; sample < sample_count; sample++)
            {

                double u = ((double)x + random_double()) / (double)width;
                double v = ((double)y + random_double()) / (double)height;
                r = get_camera_ray(&camera, u, v);
                current_color = (ccol(r, 0));
                col = vec3_add(col, current_color);
            }
            col = vec3_mul_val(col, sample_inv);
            col = vec3_create(sqrt(col.x), sqrt(col.y), sqrt(col.z));
            final_color = vec_to_color(col);

            framebuffer_lock();
            framebuffer[(int)x + (int)y * width] = final_color;
            if (stop)
            {
                framebuffer_unlock();
                return;
            }
            framebuffer_unlock();
        }
    }
}
static void *render_update_part_thread(void *arg)
{
    struct render_thread_args *args = arg;
    double u = (double)args->s_x;
    double v = (double)args->s_y;

    render_update_part(args->framebuffer, args->width, args->height, u, v, u + sample_step_x, v + sample_step_y);

    return NULL;
}

void render_update(Color *framebuffer, size_t width, size_t height)
{
    size_t i = 0;
    size_t s_x = 0;
    size_t s_y = 0;
    static const int sample_count_x = RENDER_THREAD;
    static const int sample_step_x = SCRN_WIDTH / sample_count_x;
    static const int sample_count_y = RENDER_THREAD;
    static const int sample_step_y = SCRN_HEIGHT / sample_count_y;

    while (true)
    {
        i++;
        if (thr[i] == 0)
        {
            args[i].framebuffer = framebuffer;
            args[i].width = width;
            args[i].height = height;
            args[i].s_x = s_x;
            args[i].s_y = s_y;

            pthread_create(&thr[i], NULL, render_update_part_thread, &args[i]);
        }

        i++;
        s_x += sample_step_x;
        if (s_x >= SCRN_WIDTH)
        {
            s_x = 0;
            s_y += sample_step_y;
            if (s_y >= SCRN_HEIGHT)
            {
                s_y = 0;
                break;
            }
        }
    }
}

pthread_mutex_t main_mutex;
void render_init(void)
{
    stop = false;
    add_hitable_object((HitCallback)hit_sphere_object_callback, sphere_create(0.5, vec3_create(0, 0, -1)), dieletric_create(1.5));
    add_hitable_object((HitCallback)hit_sphere_object_callback, sphere_create(100, vec3_create(0, -100.5, -1)), metal_create(vec3_create(0.8, 0.8, 0), 0.3));
    add_hitable_object((HitCallback)hit_sphere_object_callback, sphere_create(0.5, vec3_create(1, 0, -1)), metal_create(vec3_create(0.8, 0.6, 0.2), 0));
    add_hitable_object((HitCallback)hit_sphere_object_callback, sphere_create(0.5, vec3_create(-1, 0, -1)), metal_create(vec3_create(0.8, 0.8, 0.8), 5));
    pthread_mutex_init(&main_mutex, NULL);
}

void render_deinit(void)
{
    size_t i = 0;
    stop = true;
    for (i = 0; i < 256; i++)
    {
        if (thr[i] != 0)
            pthread_join(thr[i], NULL);
    }
    hit_destroy_all_objects();
    pthread_mutex_destroy(&main_mutex);
}
void framebuffer_lock(void)
{
    pthread_mutex_lock(&main_mutex);
}

void framebuffer_unlock(void)
{
    pthread_mutex_unlock(&main_mutex);
}
