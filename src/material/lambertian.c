#include "lambertian.h"
#include "../onb.h"
#include "../pdf/cosine.h"
#include "../pdf/pdf.h"
#include "../texture/solid_color.h"
#include "../utils.h"
bool lambertian_callback(const Ray *r_in, const HitRecord *record, MaterialRecord *mat_record, const Lambertian *self)
{
    Onb uvw = create_onb_from_vec(record->normal);
    (void)r_in;

    mat_record->attenuation = texture_get(record->u, record->v, &record->pos, &self->albedo);
    mat_record->pdf = make_pdf_cosine(uvw);
    mat_record->is_specular = false;

    return true;
}
rt_float lambertian_get_pdf(const Ray *r_in, const HitRecord *record, const Ray *scattered, const Lambertian *self)
{
    (void)self;
    (void)r_in;
    rt_float cosine = vec3_dot(record->normal, vec3_unit(scattered->direction));
    return cosine < 0 ? 0 : cosine / M_PI;
}
Material lambertian_create(Vec3 albedo)
{
    return lambertian_create_texture(solid_color_create(albedo));
}

Material lambertian_create_texture(Texture albedo)
{
    Material mat;
    Lambertian *self = malloc(sizeof(Lambertian));

    self->albedo = albedo;

    mat.data = self;
    mat.type = MATERIAL_LAMBERTIAN;
    return mat;
}
