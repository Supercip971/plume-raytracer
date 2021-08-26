#pragma once

/* todo: implement a better data structure */

#include "hitable.h"

typedef struct bhv_data
{
    Object left;
    Object right;
    AABB box;
} BhvData;

Object bhv_create(HitableList *list, size_t idx_start, size_t idx_end, double time_start, double time_end);
