#pragma once

#include <stddef.h>
#include "color.h"

void render_update(Color *framebuffer, size_t width, size_t height);

void render_init(void);

void render_deinit(void);

void framebuffer_lock(void);

void framebuffer_unlock(void);
