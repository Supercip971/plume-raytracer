
#include "engine.h"
#include <limits.h>
#include <math.h>
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "bvh.h"
#include "camera.h"
#include "config.h"
#include "impl.h"
#include "lock.h"
#include "matrix4x4.h"
#include "pdf/cosine.h"
#include "pdf/hitable.h"
#include "pdf/mixture.h"
#include "ray.h"
#include "utils.h"
#include "vec3.h"
#include "world/worlds.h"

struct render_thread_args
{
    Color *framebuffer;
    size_t width;
    size_t height;
    size_t s_x;
    size_t s_y;
    size_t ccount;
    int active;
    Lock lock;
};

struct render_part_args
{
    Color *framebuffer;
    Vec3 background;
    size_t width;
    size_t height;
    rt_float x_begin;
    rt_float y_begin;
    rt_float x_end;
    rt_float y_end;
    size_t sample_count;
};

static volatile size_t running_thread_count = 0;

static void increase_running_thread(void)
{
    __atomic_fetch_add(&running_thread_count, 1, __ATOMIC_SEQ_CST);
}

static void decrease_running_thread(void)
{
    __atomic_fetch_sub(&running_thread_count, 1, __ATOMIC_SEQ_CST);
}

static int sample_step_x = SCRN_WIDTH / (RENDER_THREAD);
static int sample_step_y = SCRN_HEIGHT / (RENDER_THREAD);

static volatile uint64_t thr[MAX_RENDER_THREAD] = {};

static volatile bool stop;
static Camera camera;

struct render_thread_args *args;
/* handling this as a global variable is utterly retarded */
static Object root;
static Object lights;

static Vec3 background_color;
Vec3 calculate_ray_color(Ray from, int depth, const Vec3 *background);
static Vec3 calculate_ray_color_impl(Ray from, int depth, const Vec3 *background)
{
    HitRecord record = {};
    Ray forked_ray = from;
    Vec3 emitted = vec3_create(0, 0, 0);
    rt_float pdf = 0;
    MaterialRecord mat_record = {};

    const rt_float t_max = 10000000;
    if (!object_collide(from, 0.001, t_max, &record, &root))
    {
        return *background;
    }

    material_color_emit(&record, &emitted, &record.material);

    if (!material_get(&from, &record, &mat_record, &record.material) || depth > MAX_BOUNCE_DEPTH)
    {
        return emitted;
    }

    if (mat_record.is_specular)
    {
        ray_dir_init(&mat_record.scattered);
        return vec3_mul(
            mat_record.attenuation,
            calculate_ray_color_impl(mat_record.scattered, depth + 1, background));
    }

    Pdf hitable_pdf = make_pdf_hitable(&lights, record.pos);
    Pdf mixture_pdf = make_mixture_pdf(&mat_record.pdf, &hitable_pdf);
    forked_ray.origin = record.pos;
    forked_ray.direction = pdf_generate(&mixture_pdf);

    if (is_vec3_near_zero(forked_ray.direction))
    {
        forked_ray.direction = record.normal;
    }

    forked_ray.time = from.time;
    ray_dir_init(&forked_ray);
    pdf = pdf_value(forked_ray.direction, &mixture_pdf);

    if (pdf <= 0.001)
    {
        return emitted;
    }

    rt_float scat_pdf = material_get_pdf(&from, &record, &forked_ray, &record.material);

    // emmited + (albedo * mat_pdf) * (raycol / pdf)
    Vec3 l = vec3_mul_val(
        mat_record.attenuation,
        scat_pdf);

    Vec3 r = vec3_div_val(
        calculate_ray_color_impl(forked_ray, depth + 1, background),
        pdf);

    return vec3_add(
        emitted,

        vec3_mul(
            l,
            r));

}

Vec3 calculate_ray_color(Ray from, int depth, const Vec3 *background)
{

    while (!stop)
    {
        Vec3 v = calculate_ray_color_impl(from, depth, background);
        if (isnan(v.x) || isnan(v.y) || isnan(v.z))
        {
            printf("NAN!\n");
            return (vec3_create(10000, 0, 0));
        }

        return v;
    }

    return vec3_create(0, 0, 0);
}

static Color push_color(int sample_count, Color current, Color pushed)
{
    if (sample_count != 0)
    {
        // current = (previous * (arg->sample_count + c) + current) / (arg->sample_count + c + 1);
        current = vec_to_color(vec3_div_val(
            vec3_add(
                vec3_mul_val(
                    vec_from_color(current), (rt_float)(sample_count)),
                vec_from_color(pushed)),
            sample_count + 1));
    }
    else
    {
        current = pushed;
    }
    return current;
}

FLATTEN static void render_update_part(struct render_part_args const *arg)
{
    Vec3 current_color;
    Color final_color;

    rt_float offx[SAMPLE_PER_THREAD], offy[SAMPLE_PER_THREAD];
    Color previous_color;
    Ray r;
    size_t x, y, c;
    rt_float u, v;

    rt_float inv_w = 1.0 / (arg->width - 1);
    rt_float inv_h = 1.0 / (arg->height - 1);

    for (c = 0; c < SAMPLE_PER_THREAD; c++)
    {
        offx[c] = random_rt_float();
        offy[c] = random_rt_float();
    }

    for (y = arg->y_begin; y < arg->y_end; y++)
    {
        for (x = arg->x_begin; x < arg->x_end; x++)
        {
            for (c = 0; c < SAMPLE_PER_THREAD; c++)
            {

                if (stop)
                {
                    return;
                }

                u = ((rt_float)x + offx[c]) * inv_w;
                v = ((rt_float)y + offy[c]) * inv_h;

                r = get_camera_ray(&camera, u, v);

                current_color = calculate_ray_color(r, 0, &arg->background);

                /* add current sample to sum */

                previous_color = get_pixel(arg->framebuffer, x, y, arg->width);

                final_color = push_color(arg->sample_count + c, previous_color, vec_to_color(current_color));

                set_pixel(arg->framebuffer, x, y, arg->width, final_color);
            }
        }
    }
}

static void render_init_args(struct render_part_args *arg, struct render_thread_args vargs)
{
    rt_float u = (rt_float)vargs.s_x * sample_step_x;
    rt_float v = (rt_float)vargs.s_y * sample_step_y;

    *arg = (struct render_part_args){
        .width = vargs.width,
        .height = vargs.height,
        .framebuffer = vargs.framebuffer,
        .sample_count = vargs.ccount,
        .x_begin = u,
        .x_end = u + sample_step_x,
        .y_begin = v,
        .y_end = v + sample_step_y,
        .background = background_color,
    };
}

static void *render_update_part_thread(void *thread_arg)
{
    struct render_thread_args vargs = *(struct render_thread_args *)thread_arg;

    struct render_part_args render_argument;

    render_init_args(&render_argument, vargs);

    render_update_part(&render_argument);

    render_argument.sample_count += SAMPLE_PER_THREAD;

    lock_acquire(&args[vargs.s_x + vargs.s_y * RENDER_THREAD].lock);
    args[vargs.s_x + vargs.s_y * RENDER_THREAD].ccount = vargs.ccount + SAMPLE_PER_THREAD;
    args[vargs.s_x + vargs.s_y * RENDER_THREAD].active = 0;
    lock_release(&args[vargs.s_x + vargs.s_y * RENDER_THREAD].lock);

    decrease_running_thread();
    return NULL;
}

static bool get_least_sample(size_t *sx, size_t *sy)
{
    size_t current_min = MAX_SAMPLE;
    size_t ss;
    size_t mx = 0;
    size_t my = 0;
    bool founded = false;
    for (ss = 0; ss < RENDER_THREAD * RENDER_THREAD; ss++)
    {
        lock_acquire(&args[ss].lock);

        if (args[ss].active == 0 && (args[ss].ccount <= current_min))
        {
            current_min = args[ss].ccount;
            my = ss / RENDER_THREAD;
            mx = ss % RENDER_THREAD;
            founded = true;
        }

        lock_release(&args[ss].lock);
    }

    *sx = mx;
    *sy = my;

    return founded;
}

static bool get_no_active_thread(size_t *thread_id)
{
    size_t i;
    size_t saved_i = 1000000000;
    bool founded = false;
    for (i = 0; i < MULTIPLE_THREAD; i++)
    {
        if (thr[i] == 0 || impl_is_thread_ended((uint64_t)thr[i]))
        {

            founded = true;
            saved_i = i;
        }
    }

    *thread_id = saved_i;
    return founded;
}

bool render_update(Color *framebuffer, size_t width, size_t height)
{
    size_t s_x = 0;
    size_t s_y = 0;
    size_t ii = 0;
    size_t fi = 0;

    while (running_thread_count < MULTIPLE_THREAD)
    {
        if (get_least_sample(&s_x, &s_y))
        {
            fi = s_x + s_y * RENDER_THREAD;
            lock_acquire(&args[fi].lock);
            args[fi].framebuffer = framebuffer;
            args[fi].width = width;
            args[fi].height = height;
            args[fi].s_x = s_x;
            args[fi].s_y = s_y;
            if (!get_no_active_thread(&ii))
            {
                // printf("no free thread founded \n");
                lock_release(&args[fi].lock);
                return false;
            }
            args[fi].active = 1;

            impl_start_thread((uint64_t *)&thr[ii], render_update_part_thread, (void *)(args + (fi)));

            increase_running_thread();
            lock_release(&args[fi].lock);
        }
        else
        {
            // printf("no free sample founded \n");
            if (running_thread_count != 0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        s_x = 0;
        s_y = 0;
        ii = 0;
        fi = 0;
    }
    return true;
}

void render_wait_all_thread(void)
{
    while (running_thread_count != 0)
    {
        asm volatile("pause");
    }
}

void render_init(void)
{
    size_t i = 0;
    WorldConfig conf = {};
    stop = false;
    scene_init(&root, &lights, &conf);
    camera = create_camera(conf.cam_config);
    background_color = conf.sky_color;

    args = malloc(sizeof(struct render_thread_args) * RENDER_THREAD * RENDER_THREAD);

    for (i = 0; i < RENDER_THREAD * RENDER_THREAD; i++)
    {
        memset(&(args[i]), 0, sizeof(struct render_thread_args));
    }

    for (i = 0; i < MAX_RENDER_THREAD; i++)
    {
        thr[i] = 0;
    }
}

void render_deinit(void)
{
    size_t i = 0;

    stop = true;

    for (i = 0; i < MAX_RENDER_THREAD; i++)
    {
        if (thr[i] != 0)
        {

            impl_join_thread(thr[i]);
        }
    }

    hit_destroy_all_objects(&root);
}
