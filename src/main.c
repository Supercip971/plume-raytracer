#include <SDL/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include "config.h"
#include "framebuffer_wrapper.h"
#include "utils.h"

FLATTEN int main(void)
{
    render_loop();
    return 0;
}
