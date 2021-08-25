#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include "boolean.h"
#include "config.h"
#include "engine.h"
#include "framebuffer_wrapper.h"
#include "hitable.h"
struct color *pixels;

struct raw_color *raw_pixels;

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
        framebuffer_lock();
        for (x = 0; x < SCRN_WIDTH; x++)
        {
            raw_pixels[x + ((SCRN_HEIGHT - y - 1) * SCRN_WIDTH)] = color_f_to_int(pixels[x + (y * SCRN_WIDTH)]);
        }
        framebuffer_unlock();
    }
}

static bool event_update(void)
{
    SDL_Event event;
    /* Poll for events. SDL_PollEvent() returns 0 when there are no  */
    /* more events on the event queue, our while loop will exit when */
    /* that occurs.                                                  */
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            return false;
        else if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
            return false;
    }
    return true;
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
void render_loop(void)
{
    int frames = 0;
    rt_float fps;
    uint32_t prev_ticks = SDL_GetTicks();
    SDL_Window *screen = SDL_CreateWindow("c raytracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCRN_WIDTH, SCRN_HEIGHT, SDL_WINDOW_VULKAN);
    SDL_Renderer *renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Texture *framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCRN_WIDTH, SCRN_HEIGHT);

    SDL_Init(SDL_INIT_EVERYTHING);

    framebuffer_init();
    render_init();

    while (event_update())
    {
        prev_ticks = SDL_GetTicks();

        render_update(pixels, SCRN_WIDTH, SCRN_HEIGHT);
        swap_buffer();

        SDL_UpdateTexture(framebuffer, NULL, raw_pixels,
                          SCRN_WIDTH * sizeof(struct raw_color));

        framebuffer_lock();
        SDL_RenderCopy(renderer, framebuffer, NULL, NULL);
        framebuffer_unlock();
        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);

        SDL_Delay(17 - (prev_ticks - SDL_GetTicks()));
        if (frames > 20)
        {
            frames = 0;
            fps = 1000 / (SDL_GetTicks() - prev_ticks);
            printf("fps: %f \n", (double)fps);
        }
        frames++;
    }
    render_deinit();
    framebuffer_deinit();
    SDL_DestroyTexture(framebuffer);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(screen);
    SDL_Quit();
}
