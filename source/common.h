#ifndef COMMON_H
#define COMMON_H

#include <tonc.h>

#include "debug.h"

#define SB_SIZE sizeof(SCREENBLOCK)
#define TILE_WIDTH 8
#define TILE_HEIGHT TILE_WIDTH

#define GRAVITY (int)(1.5f * (FIX_SHIFT))
#define LAVA_BOUNCE (int)(110.5f * (FIX_SHIFT))
#define TERMINAL_VY GRAVITY * 20

#define GBA_WIDTH 240
#define GBA_HEIGHT 160

#define SCROLL_X_GAIN (FIXED)(0.02f * FIX_SCALEF)
#define SCROLL_X_MAX (FIXED)(3.0f * FIX_SCALEF)
#define SCROLL_X_RATE 120

#define WIN_SCORE_THREASHOLD 2000

// Stolen https://stackoverflow.com/questions/2422712/rounding-integer-division-instead-of-truncating
#define INT_DIV_CEIL(x, y) (x + y - 1) / y

#define RAND_FLOAT(max) ((float)gba_rand_range(0, 8000) / (float)(8000) * max)

#define RECT_INTERSECTION(al, ar, at, ab, bl, br, bt, bb) (al < br && ar > bl && at < bb && ab > bt)

#define MAP_COPY(dest, source, offset)        \
	for (int i = 0; i < source##Len / 2; i++) \
	dest[i] = source[i] ? source[i] + offset : 0

#define GET_BIT(value, pos) ((value) & (1 << (pos)))
#define SET_BIT(value, set, pos) (set ? (value | 1UL << pos) : (value & ~(1UL << pos)))

#define GET_RED(value) (                \
	SET_BIT(0, GET_BIT(value, 10), 0) | \
	SET_BIT(0, GET_BIT(value, 11), 1) | \
	SET_BIT(0, GET_BIT(value, 12), 2) | \
	SET_BIT(0, GET_BIT(value, 13), 3) | \
	SET_BIT(0, GET_BIT(value, 14), 4))

#define GET_GREEN(value) (             \
	SET_BIT(0, GET_BIT(value, 5), 0) | \
	SET_BIT(0, GET_BIT(value, 6), 1) | \
	SET_BIT(0, GET_BIT(value, 7), 2) | \
	SET_BIT(0, GET_BIT(value, 8), 3) | \
	SET_BIT(0, GET_BIT(value, 9), 4))

#define GET_BLUE(value) (              \
	SET_BIT(0, GET_BIT(value, 0), 0) | \
	SET_BIT(0, GET_BIT(value, 1), 1) | \
	SET_BIT(0, GET_BIT(value, 2), 2) | \
	SET_BIT(0, GET_BIT(value, 3), 3) | \
	SET_BIT(0, GET_BIT(value, 4), 4))

#define CREATE_COLOUR(r, g, b) (    \
	SET_BIT(0, GET_BIT(b, 0), 0) |  \
	SET_BIT(0, GET_BIT(b, 1), 1) |  \
	SET_BIT(0, GET_BIT(b, 2), 2) |  \
	SET_BIT(0, GET_BIT(b, 3), 3) |  \
	SET_BIT(0, GET_BIT(b, 4), 4) |  \
	SET_BIT(0, GET_BIT(g, 0), 5) |  \
	SET_BIT(0, GET_BIT(g, 1), 6) |  \
	SET_BIT(0, GET_BIT(g, 2), 7) |  \
	SET_BIT(0, GET_BIT(g, 3), 8) |  \
	SET_BIT(0, GET_BIT(g, 4), 9) |  \
	SET_BIT(0, GET_BIT(r, 0), 10) | \
	SET_BIT(0, GET_BIT(r, 1), 11) | \
	SET_BIT(0, GET_BIT(r, 2), 12) | \
	SET_BIT(0, GET_BIT(r, 3), 13) | \
	SET_BIT(0, GET_BIT(r, 4), 14))

// #define MAX_SCROLL_X_SPEED ((FIXED))

typedef struct scene_t
{
	void (*show)(void);
	void (*update)(void);
	void (*hide)(void);
} scene_t;

extern FIXED _scroll_x;

inline void set_scroll_x(FIXED value)
{
	_scroll_x = CLAMP(value, 0, SCROLL_X_MAX);
}

void scene_set(scene_t scene);
void scene_update();

int frame_count();
void common_step();

void init_seed(int seed);
int gba_rand();

// Min and max incluscive
inline int gba_rand_range(int min, int max)
{
	return (gba_rand() % (max - min + 1)) + min;
}

void load_blank();
void hide_all_objects();

void init_score();
void clear_score();

//NOT FIXED int
void add_score(int x);
int get_score();

#endif