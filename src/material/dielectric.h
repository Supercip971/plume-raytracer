#pragma once
#include "../hitable.h"

typedef struct
{
    rt_float ref_index;
} Dieletric;

Material dieletric_create(rt_float red_index);
