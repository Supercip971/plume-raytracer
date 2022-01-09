#include "worlds.h"
#include <stdio.h>
#include "../bvh.h"
#include "../hitable.h"
#include "../image.h"
#include "../impl.h"
#include "../material/dielectric.h"
#include "../material/isotropic.h"
#include "../material/lambertian.h"
#include "../material/light.h"
#include "../material/material.h"
#include "../material/metal.h"
#include "../shape/Sphere.h"
#include "../shape/aa_rec.h"
#include "../shape/box.h"
#include "../shape/constant_medium.h"
#include "../shape/moving_sphere.h"
#include "../shape/transform.h"
#include "../texture/checker.h"
#include "../texture/image.h"
#include "../texture/noise.h"
#include "../texture/solid_color.h"
#include "../texture/texture.h"

static struct camera_config camera_config_init(Vec3 position, Vec3 lookat, rt_float vfov, bool moving_obj)
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

    return camera_config;
}

static void noise_scene(Object *root, WorldConfig *config)
{
    HitableList *lst;
    Texture per_texture = perlin_create(4);
    *root = create_hitable_list();

    add_hitable_object(root, sphere_create(1000, vec3_create(0, -1000, 0), lambertian_create_texture(per_texture)));
    add_hitable_object(root, sphere_create(2, vec3_create(0, 2, 0), lambertian_create_texture(per_texture)));

    lst = root->data;
    bvh_create_rec(lst, 1, 0);

    config->cam_config = camera_config_init(vec3_create(13, 2, 3), vec3_create(0, 0, 0), 20, false);

    config->sky_color = vec3_create(0.70, 0.80, 1);
}

static void earth_scene(Object *root, WorldConfig *config)
{
    HitableList *lst;
    *root = create_hitable_list();

    add_hitable_object(root, sphere_create(2, vec3_create(0, 0, 0), lambertian_create_texture(image_create(image_load("assets/test_colors.png")))));

    lst = root->data;
    bvh_create_rec(lst, 1, 0);

    config->cam_config = camera_config_init(vec3_create(13, 2, 3), vec3_create(0, 0, 0), 20, false);
    config->sky_color = vec3_create(0.70, 0.80, 1);
}

static void random_scene(Object *root, WorldConfig *config)
{
    int a, b;
    HitableList *lst;
    Texture checker;

    *root = create_hitable_list();

    checker = checker_create_col(vec3_create(0.2, 0.3, 0.1), vec3_create(0.9, 0.9, 0.9), 10.f);
    add_hitable_object(root, sphere_create(1000, vec3_create(0, -1000, -1),
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
                    add_hitable_object(root, sphere_create(0.2, center, result_material));
                }
                else if (material < 0.4)
                {
                    Vec3 random_albedo = vec3_create(random_rt_float(), random_rt_float(), random_rt_float());
                    result_material = lambertian_create(random_albedo);
                    add_hitable_object(root, sphere_create(0.2, center, result_material));
                }
                else if (material < 0.9)
                {
                    Vec3 random_albedo1 = vec3_create(random_rt_float(), random_rt_float(), random_rt_float());
                    Vec3 random_albedo2 = vec3_create(random_rt_float(), random_rt_float(), random_rt_float());
                    result_material = lambertian_create_texture(checker_create_col(random_albedo1, random_albedo2, 50));
                    add_hitable_object(root, sphere_create(0.2, center, result_material));
                }
                else if (material < 0.95)
                {
                    Vec3 random_albedo = vec3_create(random_rt_float(), random_rt_float(), random_rt_float());
                    rt_float fuzz = random_rt_float() / 2;
                    result_material = metal_create(random_albedo, fuzz);
                    add_hitable_object(root, sphere_create(0.2, center, result_material));
                }
                else
                {
                    result_material = dieletric_create(1.5);
                    add_hitable_object(root, sphere_create(0.2, center, result_material));
                }
            }
        }
    }

    add_hitable_object(root, sphere_create(1.0, vec3_create(0, 1, 0), dieletric_create(1.5)));
    add_hitable_object(root, sphere_create(1.0, vec3_create(-4, 1, 0), lambertian_create(vec3_create(0.4, 0.2, 0.1))));
    add_hitable_object(root, sphere_create(1.0, vec3_create(4, 1, 0), metal_create(vec3_create(0.7, 0.6, 0.5), 0)));

    lst = root->data;
    bvh_create_rec(lst, 1, 0);

    config->cam_config = camera_config_init(vec3_create(13, 2, 3), vec3_create(0, 0, 0), 20, false);
    config->sky_color = vec3_create(0.70, 0.80, 1);
}

static void light_scene(Object *root, WorldConfig *config)
{
    HitableList *lst;
    Material diff_lightred = light_create(vec3_create(16, 0, 0));
    Material diff_lightgreen = light_create(vec3_create(0, 16, 0));
    Material diff_lightblue = light_create(vec3_create(0, 0, 16));
    *root = create_hitable_list();

    add_hitable_object(root, sphere_create(1000, vec3_create(0, -1000, 0), lambertian_create(vec3_create(0.9, 0.9, 0.9))));
    add_hitable_object(root, sphere_create(2, vec3_create(0, 2, 0), dieletric_create(1.5)));
    add_hitable_object(root, sphere_create(1, vec3_create(-20, 2, 0), diff_lightred));
    add_hitable_object(root, sphere_create(1, vec3_create(-20, 2, -10), diff_lightblue));
    add_hitable_object(root, sphere_create(1, vec3_create(-20, 2, 10), diff_lightgreen));
    /* add_hitable_object(&root, sphere_create(2, vec3_create(0, 8, 0), (diff_light)));
    add_hitable_object(&root, aaxyrect_create(3, 5, 1, 3, -2, diff_light));
*/

    lst = root->data;
    bvh_create_rec(lst, 1, 0);

    config->cam_config = camera_config_init(vec3_create(26, 3, 6), vec3_create(0, 2, 0), 20, false);

    config->sky_color = vec3_create(0, 0, 0);
}

static void cornell_box(Object *root, WorldConfig *config)
{
    HitableList *lst;
    Object box;
    Matrix4x4 diff;
    Matrix4x4 diff_rot;
    Matrix4x4 diff_mov;
    Object translated_box;
    Object box2;
    Matrix4x4 diff2;
    Matrix4x4 diff2_rot;
    Matrix4x4 diff2_mov;
    Object translated_box2;
    Material red = lambertian_create(vec3_create(0.65, 0.05, 0.05));
    Material green = lambertian_create(vec3_create(0.12, 0.45, 0.15));
    Material light = light_create(vec3_create(15, 15, 15));
    Material white = lambertian_create(vec3_create(0.73, 0.73, 0.73));

    *root = create_hitable_list();

    add_hitable_object(root, aayzrect_create(0, 555, 0, 555, 555, green));
    add_hitable_object(root, aayzrect_create(0, 555, 0, 555, 0, red));
    add_hitable_object(root, aaxzrect_create(213, 343, 227, 332, 554, light));
    add_hitable_object(root, aaxzrect_create(0, 555, 0, 555, 0, white));
    add_hitable_object(root, aaxzrect_create(0, 555, 0, 555, 555, white));
    add_hitable_object(root, aaxyrect_create(0, 555, 0, 555, 555, white));

    box = box_create(vec3_create(0, 0, 0), vec3_create(165, 330, 165), white);
    create_matrix_translate(&diff_mov, 265, 0, 295);
    create_matrix_rotate_y(&diff_rot, DEG2RAD(15));
    matrix_multiply(&diff_mov, &diff_rot, &diff);
    translated_box = transform(box, diff);

    add_hitable_object(root, translated_box);

    box2 = box_create(vec3_create(0, 0, 0), vec3_create(165, 165, 165), white);
    create_matrix_translate(&diff2_mov, 130, 0, 65);
    create_matrix_rotate_y(&diff2_rot, DEG2RAD(-18));
    matrix_multiply(&diff2_mov, &diff2_rot, &diff2);
    translated_box2 = transform(box2, diff2);
    add_hitable_object(root, translated_box2);

    lst = root->data;
    bvh_create_rec(lst, 1, 0);

    config->cam_config = camera_config_init(vec3_create(278, 278, -800), vec3_create(278, 278, 0), 40, false);

    config->sky_color = vec3_create(0, 0, 0);
}

static void smoky_cornell_box(Object *root, WorldConfig *config)
{
    HitableList *lst;
    Object box;
    Matrix4x4 diff;
    Matrix4x4 diff_rot;
    Matrix4x4 diff_mov;
    Object translated_box;
    Object box2;
    Matrix4x4 diff2;
    Matrix4x4 diff2_rot;
    Matrix4x4 diff2_mov;
    Object translated_box2;
    Material red = lambertian_create(vec3_create(0.65, 0.05, 0.05));
    Material green = lambertian_create(vec3_create(0.12, 0.45, 0.15));
    Material light = light_create(vec3_create(15, 15, 15));
    Material white = lambertian_create(vec3_create(0.73, 0.73, 0.73));
    Material smoky_white = isotropic_create(solid_color_create(vec3_create(0.73, 0.73, 0.73)));
    Material smoky_black = isotropic_create(solid_color_create(vec3_create(0.02, 0.02, 0.02)));

    *root = create_hitable_list();

    add_hitable_object(root, aayzrect_create(0, 555, 0, 555, 555, green));
    add_hitable_object(root, aayzrect_create(0, 555, 0, 555, 0, red));
    add_hitable_object(root, aaxzrect_create(113, 443, 127, 432, 554, light));
    add_hitable_object(root, aaxzrect_create(0, 555, 0, 555, 0, white));
    add_hitable_object(root, aaxzrect_create(0, 555, 0, 555, 555, white));
    add_hitable_object(root, aaxyrect_create(0, 555, 0, 555, 555, white));

    box = box_create(vec3_create(0, 0, 0), vec3_create(165, 330, 165), white);
    create_matrix_translate(&diff_mov, 265, 0, 295);
    create_matrix_rotate_y(&diff_rot, DEG2RAD(15));
    matrix_multiply(&diff_mov, &diff_rot, &diff);
    translated_box = transform(box, diff);

    add_hitable_object(root, make_constant_medium(translated_box, 0.01, smoky_black));

    box2 = box_create(vec3_create(0, 0, 0), vec3_create(165, 165, 165), white);
    create_matrix_translate(&diff2_mov, 130, 0, 65);
    create_matrix_rotate_y(&diff2_rot, DEG2RAD(-18));
    matrix_multiply(&diff2_mov, &diff2_rot, &diff2);
    translated_box2 = transform(box2, diff2);
    add_hitable_object(root, make_constant_medium(translated_box2, 0.01, smoky_white));

    lst = root->data;
    bvh_create_rec(lst, 1, 0);

    config->cam_config = camera_config_init(vec3_create(278, 278, -800), vec3_create(278, 278, 0), 40, false);
    config->sky_color = vec3_create(0, 0, 0);
}

void scene_init(Object *root, WorldConfig *config)
{
    switch (SCENE_SELECT)
    {
    case SCENE_RANDOM:
    {
        random_scene(root, config);
        break;
    }

    case SCENE_NOISE:
    {
        noise_scene(root, config);
        break;
    }

    case SCENE_EARTH:
    {
        earth_scene(root, config);
        break;
    }

    case SCENE_LIGHT:
    {
        light_scene(root, config);
        break;
    }
    case SCENE_CORNELL_BOX:
    {
        cornell_box(root, config);
        break;
    }
    case SCENE_SMOKY_CORNELL_BOX:
    {
        smoky_cornell_box(root, config);
        break;
    }
    default:
        printf("not recognised config: %d\n", SCENE_SELECT);
        exit(-1);
        break;
    }
}
