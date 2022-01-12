
#include "engine.h"
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    rt_float x_from;
    rt_float y_from;
    rt_float x_max;
    rt_float y_max;
    size_t sample_count;
};

static size_t running_thread_count = 0;

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

static bool stop;
static Camera camera;

struct render_thread_args *args;
static uint64_t thr[MAX_RENDER_THREAD] = {0};

/* handling this as a global variable is utterly retarded */
static Object root;
static Object lights;

static Vec3 background_color;

static Vec3 calculate_ray_color(Ray from, int depth, const Vec3 *background)
{
    HitRecord record = {0};
    Ray forked_ray = from;
    Vec3 emitted = {0};
    rt_float pdf = 0;
    MaterialRecord mat_record = {0};

    if (stop)
    {
        return emitted;
    }

    if (depth > MAX_BOUNCE_DEPTH)
    {
        return vec3_create(0, 0, 0);
    }
    rt_float t_max = 10000000;
    if (!object_collide(from, 0.001, t_max, &record, &root))
    {
        return *background;
    }

    material_color_emit(&record, &emitted, &record.material);

    if (!material_get(&from, &record, &mat_record, &record.material))
    {
        return emitted;
    }

    Pdf hitable_pdf = make_pdf_hitable(&lights, record.pos);
    Pdf mixture_pdf = make_mixture_pdf(&hitable_pdf, &mat_record.pdf);

    forked_ray.origin = record.pos;
    forked_ray.direction = pdf_generate(&mixture_pdf);
    forked_ray.time = from.time;
    pdf = pdf_value(forked_ray.direction, &mixture_pdf) + 0.0001;
    /* quick and dirty code end */
    // emmited + (albedo * mat_pdf) * (raycol / pdf)
    (void)pdf;
    return vec3_add(
        emitted,

        vec3_mul(
            vec3_mul_val(
                mat_record.attenuation,
                material_get_pdf(&from, &record, &forked_ray, &record.material)),

            vec3_div_val(
                calculate_ray_color(forked_ray, depth + 1, background),
                pdf)));
}

FLATTEN static void render_update_part(struct render_part_args *arg)
{
    Vec3 current_color;
    Color final_color;

    Color previous_color;
    Ray r;
    size_t x, y;
    rt_float u, v;
    rt_float offx, offy;

    offx = random_rt_float() / (rt_float)(arg->width - 1);
    offy = random_rt_float() / (rt_float)(arg->height - 1);

    for (x = arg->x_from; x < arg->x_max; x += 1)
    {
        for (y = arg->y_from; y < arg->y_max; y += 1)
        {
            if (stop)
            {
                return;
            }
            u = ((rt_float)x) / (rt_float)(arg->width - 1) + offx;
            v = ((rt_float)y) / (rt_float)(arg->height - 1) + offy;

            r = get_camera_ray(&camera, u, v);

            current_color = (calculate_ray_color(r, 0, &arg->background));

            /* add current sample to sum */
            if (arg->sample_count != 0)
            {
                previous_color = get_pixel(arg->framebuffer, x, y, arg->width);

                current_color = vec3_div_val(
                    vec3_add(
                        vec3_mul_val(vec_from_color(previous_color), (rt_float)arg->sample_count), current_color),
                    arg->sample_count + 1);
            }

            final_color = vec_to_color(current_color);

            set_pixel(arg->framebuffer, x, y, arg->width, final_color);
        }
    }
    arg->sample_count += 1;
}

static void *render_update_part_thread(void *arg)
{
    struct render_thread_args vargs = *(struct render_thread_args *)arg;
    struct render_part_args render_argument = {0};
    rt_float u = (rt_float)vargs.s_x * sample_step_x;
    rt_float v = (rt_float)vargs.s_y * sample_step_y;
    int v_count = 0;

    render_argument.width = vargs.width;
    render_argument.height = vargs.height;
    render_argument.framebuffer = vargs.framebuffer;
    render_argument.sample_count = vargs.ccount;
    render_argument.x_from = u;
    render_argument.y_from = v;
    render_argument.x_max = sample_step_x + u;
    render_argument.y_max = sample_step_y + v;
    render_argument.background = background_color;

    while (v_count < SAMPLE_PER_THREAD)
    {
        render_update_part(&render_argument);
        v_count++;
        if (stop)
        {
            break;
        }
    }

    lock_acquire(&args[vargs.s_x + vargs.s_y * RENDER_THREAD].lock);
    args[vargs.s_x + vargs.s_y * RENDER_THREAD].ccount = vargs.ccount + v_count;
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
        if (thr[i] == 0 || impl_is_thread_ended(thr[i]))
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
                printf("no free thread founded \n");
                lock_release(&args[fi].lock);
                return false;
            }
            args[fi].active = 1;

            impl_start_thread(&thr[ii], render_update_part_thread, (void *)(args + (fi)));

            increase_running_thread();
            lock_release(&args[fi].lock);
        }
        else
        {
            printf("no free sample founded \n");
            stop = true;

            return false;
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
    while (true)
    {
        if (running_thread_count == 0)
        {
            break;
        }
        increase_running_thread();
        decrease_running_thread();
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
