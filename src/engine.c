
#include "engine.h"
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "bhv.h"
#include "camera.h"
#include "config.h"
#include "impl.h"
#include "material/dielectric.h"
#include "material/lambertian.h"
#include "material/light.h"
#include "material/metal.h"
#include "ray.h"
#include "shape/Sphere.h"
#include "shape/aa_rec.h"
#include "shape/moving_sphere.h"
#include "texture/checker.h"
#include "texture/image.h"
#include "texture/perlin.h"
#include "utils.h"
#include "vec3.h"

struct render_thread_args
{
    Color *framebuffer;
    size_t width;
    size_t height;
    size_t s_x;
    size_t s_y;
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

static int sample_step_x = SCRN_WIDTH / RENDER_THREAD;
static int sample_step_y = SCRN_HEIGHT / RENDER_THREAD;

static bool stop;
static Camera camera;

static struct render_thread_args args[MAX_RENDER_THREAD];
static uint64_t thr[MAX_RENDER_THREAD] = {0};

static Object root;
static Object bhv;

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
    if (!bhv.collide(from, 0.001, 100000000, &record, bhv.data))
    {
        return *background;
    }

    if (record.material.color_emition != NULL)
    {
        record.material.color_emition(record.u, record.v, &record.pos, &emitted, record.material.data);
    }

    if (record.material.material_callback(&from, &record, &attenuation, &forked_ray, record.material.data))
    {
        Vec3 next = calculate_ray_color(forked_ray, depth + 1, background);
        return vec3_add(vec3_mul(attenuation, next), emitted);
    }
    else
    {
        return emitted;
    }
}

static void render_update_part(struct render_part_args arg)
{
    Vec3 current_color;
    Color final_color;

    Color previous_color;
    Ray r;
    size_t x, y;
    rt_float u, v;

    for (x = arg.x_from; x < arg.x_max; x += 1)
    {
        for (y = arg.y_from; y < arg.y_max; y += 1)
        {
            u = ((rt_float)x + random_rt_float()) / (rt_float)(arg.width - 1);
            v = ((rt_float)y + random_rt_float()) / (rt_float)(arg.height - 1);

            r = get_camera_ray(&camera, u, v);

            current_color = (calculate_ray_color(r, 0, &arg.background));

            /* add current sample to sum */
            if (arg.sample_count != 0)
            {
                previous_color = get_pixel(arg.framebuffer, x, y, arg.width);

                current_color = vec3_div_val(
                    vec3_add(
                        vec3_mul_val(vec_from_color(previous_color), arg.sample_count), current_color),
                    arg.sample_count + 1);
            }

            final_color = vec_to_color(current_color);

            set_pixel(arg.framebuffer, x, y, arg.width, final_color);

            if (stop)
            {
                return;
            }
        }
    }
}

static void *render_update_part_thread(void *arg)
{
    struct render_thread_args *args = arg;
    struct render_part_args render_argument;
    size_t tick_start = impl_get_tick();
    rt_float u = (rt_float)args->s_x;
    rt_float v = (rt_float)args->s_y;

    render_argument.sample_count = 0;
    render_argument.width = args->width;
    render_argument.height = args->height;
    render_argument.framebuffer = args->framebuffer;
    render_argument.x_from = u;
    render_argument.y_from = v;
    render_argument.x_max = sample_step_x + u;
    render_argument.y_max = sample_step_y + v;
    render_argument.background = background_color;

    while (render_argument.sample_count < MAX_SAMPLE)
    {
        render_update_part(render_argument);
        render_argument.sample_count++;
    }

    printf("thread ended [!] %li \n", impl_get_tick() - tick_start);

    return NULL;
}

void render_update(Color *framebuffer, size_t width, size_t height)
{
    size_t i = 0;
    size_t s_x = 0;
    size_t s_y = 0;

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

            impl_start_thread(&thr[i], render_update_part_thread, &args[i]);
        }

        i++;
        s_x += sample_step_x;
        if (s_x >= SCRN_WIDTH)
        {
            s_x = 0;
            s_y += sample_step_y;
            if (s_y >= SCRN_HEIGHT)
            {
                break;
            }
        }
    }
}

static void camera_init(Vec3 position, Vec3 lookat, rt_float vfov)
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
    bhv = bhv_create(lst, 0, lst->child_count, 0.0, 1.0);

    camera_init(vec3_create(13, 2, 3), vec3_create(0, 0, 0), 20);

    background_color = vec3_create(0.70, 0.80, 1);
}
static void earth_scene(void)
{
    HitableList *lst;
    root = create_hitable_list();

    add_hitable_object(&root, sphere_create(2, vec3_create(0, 0, 0), lambertian_create_texture(image_create(image_load("assets/test_colors.png")))));

    lst = root.data;
    bhv = bhv_create(lst, 0, lst->child_count, 0.0, 1.0);

    camera_init(vec3_create(13, 2, 3), vec3_create(0, 0, 0), 20);
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
                    Vec3 center2 = vec3_create(center.x, center.y + random_rt_float() * 0.5, center.z);
                    Vec3 random_albedo = vec3_create(random_rt_float(), random_rt_float(), random_rt_float());
                    result_material = lambertian_create(vec3_mul(random_albedo, random_albedo));
                    add_hitable_object(&root, moving_sphere_create(0.2, 0, 1, center, center2, result_material));
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
    bhv = bhv_create(lst, 0, lst->child_count, 0.0, 1.0);

    camera_init(vec3_create(13, 2, 3), vec3_create(0, 0, 0), 20);
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
    add_hitable_object(&root, aarect_create(3, 5, 1, 3, -2, diff_light));
*/

    lst = root.data;
    bhv = bhv_create(lst, 0, lst->child_count, 0.0, 1.0);

    camera_init(vec3_create(26, 3, 6), vec3_create(0, 2, 0), 20);

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
    default:
        break;
    }
}

void render_init(void)
{
    stop = false;

    scene_init();
}

void render_deinit(void)
{
    size_t i = 0;

    stop = true;

    for (i = 0; i < MAX_RENDER_THREAD; i++)
    {
        if (thr[i] != 0)
            impl_join_thread(thr[i]);
    }

    hit_destroy_all_objects(&root);
}
