#pragma once

#define SCRN_WIDTH 1920
#define SCRN_HEIGHT 1080

/* use render_thread ^2 */
/* if you put 4 it will use (4^2) 16 threads */
#define RENDER_THREAD 4

#define MAX_SAMPLE 10000000
#define MAX_RENDER_THREAD 256

#define MAX_BOUNCE_DEPTH 64

typedef float rt_float;

/* use the scene with a lot off random placed balls */
#define SCENE_RANDOM 0

/* show off the noise support */
#define SCENE_NOISE 1

/* show basic texture support */

#define SCENE_EARTH 2

/* show basic noise + light support */

#define SCENE_LIGHT 3

#define SCENE_SELECT SCENE_LIGHT
