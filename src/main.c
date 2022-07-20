#include <SDL/SDL.h>
#include <engine.h>
#include <impl.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <utils/config.h>
#include <utils/utils.h>

FLATTEN int main(void)
{

    struct timeval stop, start;
    gettimeofday(&start, NULL);
    // do stuff

    impl_render_start();
    render_init();
    impl_render_loop();
    render_deinit();
    impl_render_stop();

    gettimeofday(&stop, NULL);
    printf("took %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    return 0;
}
