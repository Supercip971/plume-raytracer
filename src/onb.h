#ifndef ONB_H
#define ONB_H
#include "vec3.h"
/* we may use a 4x4 matrix instead of an ONB, but this is faster and simpler */

typedef struct onb
{
    Vec3 u;
    Vec3 v;
    Vec3 w;
} Onb;

Vec3 onb_local(const Onb *self, Vec3 vec);

Onb create_onb_from_vec(Vec3 vec);

#endif
