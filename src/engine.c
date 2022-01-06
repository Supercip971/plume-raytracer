
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
#include "material/dielectric.h"
#include "material/lambertian.h"
#include "material/light.h"
#include "material/metal.h"
#include "ray.h"
#include "shape/Sphere.h"
#include "shape/aa_rec.h"
#include "shape/box.h"
#include "shape/moving_sphere.h"
#include "texture/checker.h"
#include "texture/image.h"
#include "texture/noise.h"
#include "utils.h"
#include "vec3.h"

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

static Object root;

static Vec3 background_color;

static Vec3 calculate_ray_color(Ray from, int depth, const Vec3 *background)
{
    HitRecord record = {0};
    Ray forked_ray = from;
    Vec3 attenuation = vec3_create(1, 1, 1);
    Vec3 emitted = {0};

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

    material_color_emit(record.u, record.v, &record.pos, &emitted, &record.material);

    if (material_get(&from, &record, &attenuation, &forked_ray, &record.material))
    {
        Vec3 next = calculate_ray_color(forked_ray, depth + 1, background);
        return vec3_add(vec3_mul(attenuation, next), emitted);
    }
    return emitted;
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
                        vec3_mul_val(vec_from_color(previous_color), arg->sample_count), current_color),
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

static void camera_init(Vec3 position, Vec3 lookat, rt_float vfov, bool moving_obj)
{
    struct camera_config camera_config;

    camera_config.position = position;
    camera_config.lookat = lookat;
    camera_config.up = vec3_create(0, 1, 0);
    camera_config.aspect = ((rt_float)SCRN_WIDTH / (rt_float)SCRN_HEIGHT);
    camera_config.vfov = vfov;
    camera_config.aperture = 0.0001;
    camera_config.focus_distance = 10;
    camera_config.time_end = 1;
    camera_config.time_start = 0.0;
    camera_config.moving_obj = moving_obj;
    camera = create_camera(camera_config);
}
static void noise_scene(void)
{
    HitableList *lst;
    Texture per_texture = perlin_create(4);
    root = create_hitable_list();

    add_hitable_object(&root, sphere_create(1000, vec3_create(0, -1000, 0), lambertian_create_texture(per_texture)));
    add_hitable_object(&root, sphere_create(2, vec3_create(0, 2, 0), lambertian_create_texture(per_texture)));

    lst = root.data;
    bvh_create_rec(lst, 1, 0);

    camera_init(vec3_create(13, 2, 3), vec3_create(0, 0, 0), 20, false);

    background_color = vec3_create(0.70, 0.80, 1);
}
static void earth_scene(void)
{
    HitableList *lst;
    root = create_hitable_list();

    add_hitable_object(&root, sphere_create(2, vec3_create(0, 0, 0), lambertian_create_texture(image_create(image_load("assets/test_colors.png")))));

    lst = root.data;
    bvh_create_rec(lst, 1, 0);

    camera_init(vec3_create(13, 2, 3), vec3_create(0, 0, 0), 20, false);
    background_color = vec3_create(0.70, 0.80, 1);
}

static void random_scene(void)
{
    int a, b;
    HitableList *lst;
    Texture checker;

    root = create_hitable_list();

    checker = checker_create_col(vec3_create(0.2, 0.3, 0.1), vec3_create(0.9, 0.9, 0.9), 10.f);
    add_hitable_object(&root, sphere_create(1000, vec3_create(0, -1000, -1),
                                            lambertian_create_texture(checker)));

    for (a = -11; a < 11; a++)
    {
        for (b = -11; b < 11; b++)
        {
            Material result_material;

            rt_float material = random_rt_float();
            Vec3 center = vec3_create(a + 0.9 * random_rt_float(), 0.2, b + 0.9 * random_rt_float());

            if (vec3_length(vec3_sub(center, vec3_create(4, 0.2, 0))) > 0.9)
            {
                if (material < 0.1)
                {
                    Vec3 random_albedo = vec3_create(random_rt_float(), random_rt_float(), random_rt_float());
                    result_material = lambertian_create(vec3_mul(random_albedo, random_albedo));
                    add_hitable_object(&root, sphere_create(0.2, center, result_material));
                }
                else if (material < 0.4)
                {
                    Vec3 random_albedo = vec3_create(random_rt_float(), random_rt_float(), random_rt_float());
                    result_material = lambertian_create(random_albedo);
                    add_hitable_object(&root, sphere_create(0.2, center, result_material));
                }
                else if (material < 0.9)
                {
                    Vec3 random_albedo1 = vec3_create(random_rt_float(), random_rt_float(), random_rt_float());
                    Vec3 random_albedo2 = vec3_create(random_rt_float(), random_rt_float(), random_rt_float());
                    result_material = lambertian_create_texture(checker_create_col(random_albedo1, random_albedo2, 50));
                    add_hitable_object(&root, sphere_create(0.2, center, result_material));
                }
                else if (material < 0.95)
                {
                    Vec3 random_albedo = vec3_create(random_rt_float(), random_rt_float(), random_rt_float());
                    rt_float fuzz = random_rt_float() / 2;
                    result_material = metal_create(random_albedo, fuzz);
                    add_hitable_object(&root, sphere_create(0.2, center, result_material));
                }
                else
                {
                    result_material = dieletric_create(1.5);
                    add_hitable_object(&root, sphere_create(0.2, center, result_material));
                }
            }
        }
    }

    add_hitable_object(&root, sphere_create(1.0, vec3_create(0, 1, 0), dieletric_create(1.5)));
    add_hitable_object(&root, sphere_create(1.0, vec3_create(-4, 1, 0), lambertian_create(vec3_create(0.4, 0.2, 0.1))));
    add_hitable_object(&root, sphere_create(1.0, vec3_create(4, 1, 0), metal_create(vec3_create(0.7, 0.6, 0.5), 0)));

    lst = root.data;
    bvh_create_rec(lst, 1, 0);

    camera_init(vec3_create(13, 2, 3), vec3_create(0, 0, 0), 20, false);
    background_color = vec3_create(0.70, 0.80, 1);
}
static void light_scene(void)
{
    HitableList *lst;
    Material diff_lightred = light_create(vec3_create(16, 0, 0));
    Material diff_lightgreen = light_create(vec3_create(0, 16, 0));
    Material diff_lightblue = light_create(vec3_create(0, 0, 16));
    root = create_hitable_list();

    add_hitable_object(&root, sphere_create(1000, vec3_create(0, -1000, 0), lambertian_create(vec3_create(0.9, 0.9, 0.9))));
    add_hitable_object(&root, sphere_create(2, vec3_create(0, 2, 0), dieletric_create(1.5)));
    add_hitable_object(&root, sphere_create(1, vec3_create(-20, 2, 0), diff_lightred));
    add_hitable_object(&root, sphere_create(1, vec3_create(-20, 2, -10), diff_lightblue));
    add_hitable_object(&root, sphere_create(1, vec3_create(-20, 2, 10), diff_lightgreen));
    /* add_hitable_object(&root, sphere_create(2, vec3_create(0, 8, 0), (diff_light)));
    add_hitable_object(&root, aaxyrect_create(3, 5, 1, 3, -2, diff_light));
*/

    lst = root.data;
    bvh_create_rec(lst, 1, 0);

    camera_init(vec3_create(26, 3, 6), vec3_create(0, 2, 0), 20, false);

    background_color = vec3_create(0, 0, 0);
}

static void cornell_box(void)
{
    HitableList *lst;
    Material red = lambertian_create(vec3_create(0.65, 0.05, 0.05));
    Material green = lambertian_create(vec3_create(0.12, 0.45, 0.15));
    Material light = light_create(vec3_create(15, 15, 15));
    Material white = lambertian_create(vec3_create(0.73, 0.73, 0.73));

    root = create_hitable_list();

    add_hitable_object(&root, aayzrect_create(0, 555, 0, 555, 555, green));
    add_hitable_object(&root, aayzrect_create(0, 555, 0, 555, 0, red));
    add_hitable_object(&root, aaxzrect_create(213, 343, 227, 332, 554, light));
    add_hitable_object(&root, aaxzrect_create(0, 555, 0, 555, 0, white));
    add_hitable_object(&root, aaxzrect_create(0, 555, 0, 555, 555, white));
    add_hitable_object(&root, aaxyrect_create(0, 555, 0, 555, 555, white));

    add_hitable_object(&root, box_create(vec3_create(130, 0, 65), vec3_create(295, 165, 230), white));
    add_hitable_object(&root, box_create(vec3_create(265, 0, 295), vec3_create(430, 330, 460), white));

    lst = root.data;
    bvh_create_rec(lst, 1, 0);

    camera_init(vec3_create(278, 278, -800), vec3_create(278, 278, 0), 40, false);

    background_color = vec3_create(0, 0, 0);
}

static void scene_init(void)
{
    switch (SCENE_SELECT)
    {
    case SCENE_RANDOM:
    {
        random_scene();
        break;
    }

    case SCENE_NOISE:
    {
        noise_scene();
        break;
    }

    case SCENE_EARTH:
    {
        earth_scene();
        break;
    }

    case SCENE_LIGHT:
    {
        light_scene();
        break;
    }
    case SCENE_CORNELL_BOX:
    {
        cornell_box();
        break;
    }
    default:
        break;
    }
}

void render_init(void)
{
    size_t i = 0;
    stop = false;

    scene_init();

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
