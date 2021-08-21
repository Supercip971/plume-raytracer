#include <SDL2/SDL.h>
#include <stdbool.h>
#include "config.h"
#include "engine.h"
#include "framebuffer_wrapper.h"

struct color *pixels;

struct raw_color *raw_pixels;
#define set_pixel(_X, _Y, _COLOR) pixels[_X + (_Y * SCRN_WIDTH)] = _COLOR;

void swap_buffer()
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

bool event_update()
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

void framebuffer_init()
{
    raw_pixels = malloc(sizeof(struct raw_color) * SCRN_WIDTH * SCRN_HEIGHT);
    pixels = malloc(sizeof(struct color) * SCRN_WIDTH * SCRN_HEIGHT);
}

void framebuffer_deinit()
{
    free(pixels);
    free(raw_pixels);
}
void render_loop()
{
    int frames = 0;
    float fps;
    uint32_t prev_ticks = SDL_GetTicks();

    SDL_Window *screen = SDL_CreateWindow("c raytracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCRN_WIDTH, SCRN_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_Renderer *renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Texture *framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCRN_WIDTH, SCRN_HEIGHT);

    SDL_Init(SDL_INIT_EVERYTHING);
    framebuffer_init();

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

    framebuffer_deinit();
    SDL_DestroyTexture(framebuffer);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(screen);

    SDL_Quit();
}
