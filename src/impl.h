#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "color.h"
#include "image.h"

#ifndef NOT_USE_THREAD

int impl_start_thread(uint64_t *id, void *(*func)(void *), void *args);

int impl_join_thread(uint64_t id);

#endif
/* here is the implementation of every needed function */

void impl_render_start(void);

void impl_render_loop(void);

void impl_render_stop(void);

bool impl_should_exit(void);

size_t impl_get_tick(void); /* used for fps 1 tick = 0.001 s */

#define set_pixel(FB, X, Y, WIDTH, COLOR) \
    (FB)[X + Y * WIDTH] = COLOR

#define get_pixel(FB, X, Y, WIDTH) \
    (FB)[X + Y * WIDTH]

Image image_load(const char *path);

void image_destroy(Image *img);
