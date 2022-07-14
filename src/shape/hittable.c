#include "hittable.h"

void set_face_normal(const Ray *r, const Vec3 outward, HitRecord *self)
{
    self->outward = outward;
    self->front_face = vec3_dot(r->direction, outward) < 0;
    self->normal = self->front_face ? outward : vec3_neg(outward);
}
