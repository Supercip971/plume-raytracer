#include "onb.h"

Vec3 onb_local(const Onb *self, Vec3 vec)
{
    return vec3_add(
        vec3_mul_val(self->u, vec.x),
        vec3_add(
            vec3_mul_val(self->v, vec.y),
            vec3_mul_val(self->w, vec.z)));
}

Onb create_onb_from_vec(Vec3 vec)
{
    Onb self = {};
    Vec3 a = {};
    // printf("onb vec: ");
    print_vec3(vec);
    self.w = vec3_unit(vec);
    a = (rt_abs(self.w.x) > 0.9) ? vec3_create(0, 1, 0) : vec3_create(1, 0, 0);

    self.v = vec3_unit(vec3_cross(self.w, a));
    self.u = vec3_cross(self.v, self.w);
    return self;
}
