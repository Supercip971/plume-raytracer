#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "config.h"
#include "engine.h"
#include "hitable.h"
#include "impl.h"
#include "pthread.h"
struct color *pixels;

struct raw_color *raw_pixels;
bool should_exit = false;
SDL_Window *screen;
SDL_Renderer *renderer;
SDL_Texture *framebuffer;

struct raw_color
{
    uint8_t a;
    uint8_t b;
    uint8_t g;
    uint8_t r;
} __attribute__((packed));

static rt_float color_clamp(rt_float value)
{
    if (value > 0.999)
    {
        return 1;
    }
    else if (value < 0.0)
    {
        return 0;
    }
    return value;
}

static struct raw_color
color_f_to_int(Color color)
{
    struct raw_color res;
    res.a = (uint8_t)(255);
    res.g = (uint8_t)(color_clamp(sqrt(color.g)) * 255);
    res.b = (uint8_t)(color_clamp(sqrt(color.b)) * 255);
    res.r = (uint8_t)(color_clamp(sqrt(color.r)) * 255);
    return res;
}

static void
swap_buffer(void)
{
    size_t x;
    size_t y;

    for (y = 0; y < SCRN_HEIGHT; y++)
    {
        for (x = 0; x < SCRN_WIDTH; x++)
        {
            raw_pixels[x + ((SCRN_HEIGHT - y - 1) * SCRN_WIDTH)] = color_f_to_int(pixels[x + (y * SCRN_WIDTH)]);
        }
    }
}

static bool event_update(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            should_exit = true;
            return !should_exit;
        }
        else if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        {
            should_exit = true;
            return !should_exit;
        }
    }

    return !should_exit;
}

bool impl_should_exit(void)
{
    return should_exit;
}

static void framebuffer_init(void)
{
    raw_pixels = malloc(sizeof(struct raw_color) * SCRN_WIDTH * (SCRN_HEIGHT * 2));
    pixels = malloc(sizeof(struct color) * SCRN_WIDTH * (SCRN_HEIGHT * 2));
}

static void framebuffer_deinit(void)
{
    free(pixels);
    free(raw_pixels);
}

size_t impl_get_tick(void)
{
    return SDL_GetTicks();
}

void impl_render_loop(void)
{
    int frames = 0;
    rt_float fps;
    uint32_t prev_ticks = 0;

    while (event_update())
    {
        prev_ticks = impl_get_tick();

        render_update(pixels, SCRN_WIDTH, SCRN_HEIGHT);
        swap_buffer();

        SDL_UpdateTexture(framebuffer, NULL, raw_pixels,
                          SCRN_WIDTH * sizeof(struct raw_color));

        SDL_RenderCopy(renderer, framebuffer, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);

        SDL_Delay(17 - (prev_ticks - impl_get_tick()));

        if (frames > 20)
        {
            frames = 0;
            fps = 1000 / (impl_get_tick() - prev_ticks);
            printf("fps: %f \n", (double)fps);
        }

        frames++;
    }
}

Image image_load(const char *path)
{
    Image result;
    result.width = 0;
    result.height = 0;
    result.source_data = NULL;
    result.source_data = NULL;
    (void)path;
    return result;
}

void image_destroy(Image *img)
{
    if (img->source_data)
    {
        free(img->source_data);
    }
    if (img->converted_data)
    {
        free(img->converted_data);
    }
}

void impl_render_start(void)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    screen = SDL_CreateWindow("c raytracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCRN_WIDTH, SCRN_HEIGHT, SDL_WINDOW_VULKAN);
    renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCRN_WIDTH, SCRN_HEIGHT);

    framebuffer_init();
}

void impl_render_stop(void)
{
    framebuffer_deinit();
    SDL_DestroyTexture(framebuffer);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(screen);
    SDL_Quit();
}

int impl_start_thread(uint64_t *id, void *(*func)(void *), void *args)
{
    return pthread_create((pthread_t *)id, NULL, func, args);
}

int impl_join_thread(uint64_t id)
{
    return pthread_join(id, NULL);
}
