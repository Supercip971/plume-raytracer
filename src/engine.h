#pragma once

#include <stdbool.h>
#include <stddef.h>
#include "color.h"

bool render_update(Color *framebuffer, size_t width, size_t height);

void render_init(void);

void render_deinit(void);

void render_wait_all_thread(void);
