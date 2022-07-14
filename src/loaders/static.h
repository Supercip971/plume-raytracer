#ifndef WORLDS_H
#define WORLDS_H
#include <ds/bvh/bvh.h>
#include <gfx/camera.h>
#include <math/vec3.h>

typedef struct
{
    Vec3 sky_color;
    CameraConfig cam_config;
} WorldConfig;

void static_scene_init(Object *root, Object *light, WorldConfig *config);

#endif // WORLDS_H
