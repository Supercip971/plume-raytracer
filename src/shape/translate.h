#ifndef TRANSLATE_H
#define TRANSLATE_H
#include "../aabb.h"
#include "../hitable.h"
#include "../ray.h"
#include "../vec3.h"

typedef struct translate
{
    Vec3 offset;
    Vec3 rotation;
    Object target;
} Translate;

Object translate(Object translated, Vec3 displacement, Vec3 rotation);

#endif
