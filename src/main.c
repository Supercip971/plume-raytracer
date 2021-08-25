#include <SDL/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include "config.h"
#include "utils.h"
#include "framebuffer_wrapper.h"

FLATTEN int main(void)
{
    render_loop();
    return 0;
}
