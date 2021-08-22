#include <SDL2/SDL.h>
#include "boolean.h"
#include "config.h"
#include "engine.h"
#include "framebuffer_wrapper.h"
#include "hitable.h"
struct color *pixels;

struct raw_color *raw_pixels;
#define set_pixel(_X, _Y, _COLOR) pixels[_X + (_Y * SCRN_WIDTH)] = _COLOR;

struct raw_color
{
    uint8_t a;
    uint8_t b;
    uint8_t g;
    uint8_t r;
} __attribute__((packed));

static struct raw_color
color_f_to_int(Color color)
{
    struct raw_color res;
    res.a = (uint8_t)(color.a * 255.99);
    res.g = (uint8_t)(color.g * 255.99);
    res.b = (uint8_t)(color.b * 255.99);
    res.r = (uint8_t)(color.r * 255.99);
    return res;
}
static void
swap_buffer(void)
{
    size_t x;
    size_t y;
    for (x = 0; x < SCRN_WIDTH; x++)
    {
        for (y = 0; y < SCRN_HEIGHT; y++)
        {
            raw_pixels[x + (y * SCRN_WIDTH)] = color_f_to_int(pixels[x + (y * SCRN_WIDTH)]);
        }
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
    raw_pixels = malloc(sizeof(struct raw_color) * SCRN_WIDTH * SCRN_HEIGHT);
    pixels = malloc(sizeof(struct color) * SCRN_WIDTH * SCRN_HEIGHT);
}

static void framebuffer_deinit(void)
{
    free(pixels);
    free(raw_pixels);
}
void render_loop(void)
{
    int frames = 0;
    float fps;
    uint32_t prev_ticks = SDL_GetTicks();
    SDL_Window *screen = SDL_CreateWindow("c raytracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCRN_WIDTH, SCRN_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(screen, -1, 0);
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
        SDL_RenderCopy(renderer, framebuffer, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);

        if (frames > 20)
        {
            frames = 0;
            fps = 1000 / (SDL_GetTicks() - prev_ticks);
            printf("fps: %f \n", fps);
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
