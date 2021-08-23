#pragma once
#include <stdlib.h>

#define random_float() \
    (float)((float)random() / (float)RAND_MAX)
