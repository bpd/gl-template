#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GL/glfw.h>


#ifdef _MSC_VER

typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;

#else
#include <stdint.h>
#endif

#include <math.h>

#define FRAMES_PER_SECOND 60

static const uint32_t MS_BETWEEN_RENDERS = (1000/FRAMES_PER_SECOND);

static const int screen_width = 640;
static const int screen_height = 480;

typedef struct
{
  long double x, y;
}
Vector;

static const Vector ZERO_VECTOR = { 0, 0 };

static const Vector GRAVITY = { 0, -9.8f };

static const Vector MOVE_LEFT = { -2, 0 };

static const Vector MOVE_RIGHT = { -2, 0 };

static const Vector JUMP = { 0, 15 };

typedef struct
{
  long double x, y;
  int32_t width, height;

  Vector a;   /* acceleration in m/s^2 */
  Vector v;   /* velocity in m/s */
  long double m;   /* mass in kg */

  int engine_firing;

} Ship;

typedef Ship Particle;


void game_main_loop();

void quit_game(int code);