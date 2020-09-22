#ifndef COMMON_H
#include <tonc.h>

#define SB_SIZE sizeof(SCREENBLOCK)
static const int GRAVITY = (int)(1.0f * (FIX_SHIFT));

typedef struct scene_t {
	void (*show)(void);
	void (*update)(void);
	void (*hide)(void);
} scene_t;

extern const scene_t main_game, title_screen;

extern FIXED _scroll_x;

void scene_set(scene_t scene);
void scene_update();

int frame_count();
void common_step();

void init_seed(int seed);
int gba_rand();

inline int gba_rand_range(int min, int max) {
	return (gba_rand() % (min - max + 1)) + min;
}

#endif