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
#include "ray.h"
#include "shapes.h"
#include "utils.h"
#include "vec3.h"

/* https://www.realtimerendering.com/raytracing/Ray%20Tracing%20in%20a%20Weekend.pdf */

const int sample_count_x = RENDER_THREAD;
const int sample_step_x = SCRN_WIDTH / sample_count_x;
const int sample_count_y = RENDER_THREAD;
const int sample_step_y = SCRN_HEIGHT / sample_count_y;

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

static Vec3 ccol(Ray from)
{
    HitRecord record = {0};
    double t;
    Vec3 unit_direction;

    if (hit_call_all_object(from, 0.0, 100000000, &record))
    {
        Vec3 target = vec3_add(record.pos, vec3_add(record.normal, random_vec3_unit()));
        Ray forked_ray = from;
        forked_ray.origin = record.pos;
        forked_ray.direction = vec3_sub(target, record.pos);

        return vec3_mul_val(ccol(forked_ray), 0.5);
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

    const int sample_count = 128;
    const int sample_step = (int)sample_count / 2;

    Camera camera = get_camera_default();
    Vec3 current_color;
    Color final_color;
    Ray r;
    double x, y;
    int sample;

    for (x = x_from; x < x_max; x += 1.f)
    {
        for (y = y_from; y < y_max; y += 1.f)
        {
            Vec3 col = {0};
            for (sample = 0; sample < sample_count; sample++)
            {

                double u = (x + ((double)(sample % sample_step) / sample_count)) / (double)width;
                double v = (y + ((double)(sample / sample_step) / sample_count)) / (double)height;
                r = get_camera_ray(&camera, u, v);
                current_color = (ccol(r));
                col = vec3_add(col, current_color);
            }
            col = vec3_div_val(col, sample_count);
            col = vec3_create(sqrt(col.x), sqrt(col.y), sqrt(col.z));
            final_color = vec_to_color(col);

            framebuffer_lock();
            framebuffer[(int)x + (int)y * width] = final_color;
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
            pthread_detach(thr[i]);
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
    add_hitable_object((HitCallback)hit_sphere_object_callback, sphere_create(0.5, vec3_create(0, 0, -1)));
    add_hitable_object((HitCallback)hit_sphere_object_callback, sphere_create(100, vec3_create(0, -100.5, -1)));
    pthread_mutex_init(&main_mutex, NULL);
}

void render_deinit(void)
{
    size_t i = 0;
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
