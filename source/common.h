#ifndef COMMON_H
#define COMMON_H

#include <tonc.h>

#define SB_SIZE sizeof(SCREENBLOCK)
#define TILE_WIDTH 8
#define TILE_HEIGHT TILE_WIDTH

#define GRAVITY (int)(1.5f * (FIX_SHIFT))
#define LAVA_BOUNCE (int)(110.5f * (FIX_SHIFT))
#define TERMINAL_VY GRAVITY * 20

#define GBA_WIDTH 240
#define GBA_HEIGHT 160

#define X_SCROLL_GAIN (FIXED)(0.01f * (FIX_SCALEF))
#define X_SCROLL_MAX (FIXED)(3.5f * (FIX_SCALEF))
#define X_SCROLL_RATE 120

//Stolen https://stackoverflow.com/questions/2422712/rounding-integer-division-instead-of-truncating
#define INT_DIV_CEIL(x, y) (x + y - 1) / y

typedef struct scene_t
{
	void (*show)(void);
	void (*update)(void);
	void (*hide)(void);
} scene_t;

extern FIXED _scroll_x;

void scene_set(scene_t scene);
void scene_update();

int frame_count();
void common_step();

void init_seed(int seed);
int gba_rand();

//Min and max incluscive
inline int gba_rand_range(int min, int max)
{
	return (gba_rand() % (max - min + 1)) + min;
}

void init_score();
void clear_score();

//NOT FIXED int
void add_score(int x);
int get_score();

#endif