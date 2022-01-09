#ifndef WORLDS_H
#define WORLDS_H
#include "../bvh.h"
#include "../camera.h"
#include "../vec3.h"

typedef struct
{
    Vec3 sky_color;
    CameraConfig cam_config;
} WorldConfig;

void scene_init(Object *root, WorldConfig *config);
#endif // WORLDS_H
