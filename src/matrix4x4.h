#ifndef MATRIX_H
#define MATRIX_H
#include "ray.h"
#include "utils.h"
#include "vec3.h"
typedef struct
{
    rt_float value[4][4];
} Matrix4x4;

void create_matrix_identity(Matrix4x4 *matrix);

void create_matrix_scale(Matrix4x4 *matrix, rt_float x, rt_float y, rt_float z);
void create_matrix_translate(Matrix4x4 *matrix, rt_float x, rt_float y, rt_float z);
void create_matrix_rotate_x(Matrix4x4 *matrix, rt_float angle);
void create_matrix_rotate_y(Matrix4x4 *matrix, rt_float angle);
void create_matrix_rotate_z(Matrix4x4 *matrix, rt_float angle);

void matrix_inverse(const Matrix4x4 *matrix, Matrix4x4 *result);
void matrix_transpose(const Matrix4x4 *matrix, Matrix4x4 *result);
void matrix_multiply(const Matrix4x4 *a, const Matrix4x4 *b, Matrix4x4 *result);

void matrix_apply_ray(const Matrix4x4 *matrix, Ray *ray);
void matrix_apply_vector(const Matrix4x4* matrix, Vec3 *vector);
void matrix_apply_point(const Matrix4x4* matrix, Vec3 *point);
#endif /* FE439C8D_4B4A_492C_81C7_2F76373A7F98 */
