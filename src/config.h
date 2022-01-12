#pragma once

#define PIXELS_AA (1)
#define ASCRN_WIDTH (1080)
#define ASCRN_HEIGHT (1080)
#define SCRN_WIDTH (int)(ASCRN_WIDTH * PIXELS_AA)
#define SCRN_HEIGHT (int)(ASCRN_HEIGHT * PIXELS_AA)

/* use render_thread ^2 */
/* if you put 4 it will use (4^2) 16 threads */
#define RENDER_THREAD (8)
#define MULTIPLE_THREAD (12)

#define MAX_SAMPLE 5000000
#define SAMPLE_PER_THREAD 4
#define MAX_RENDER_THREAD 256
#define MAX_RENDER_THREAD 256

#define MAX_BOUNCE_DEPTH 128

typedef float rt_float;

/* use the scene with a lot off random placed balls */
#define SCENE_RANDOM 0

/* show off the noise support */
#define SCENE_NOISE 1

/* show basic texture support */

#define SCENE_EARTH 2

/* show basic noise + light support */

#define SCENE_LIGHT 3

/* the popular cowornel bowox */

#define SCENE_CORNELL_BOX 4

/* the famous cornell box with smoke  */

#define SCENE_SMOKY_CORNELL_BOX 5

/* a random scene presenting everything in chapter 1-2 */

#define SCENE_RAND_CHAP_2 6
#define SCENE_SELECT SCENE_CORNELL_BOX

#define MAX_OBJECTS 256
