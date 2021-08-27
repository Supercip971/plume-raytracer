#include <SDL/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include "config.h"
#include "engine.h"
#include "impl.h"
#include "utils.h"

FLATTEN int main(void)
{
    impl_render_start();
    render_init();
    impl_render_loop();
    render_deinit();
    impl_render_stop();
    return 0;
}
