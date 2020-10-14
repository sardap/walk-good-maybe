#ifndef COMMON_H
#define COMMON_H

#include <tonc.h>

#define SB_SIZE sizeof(SCREENBLOCK)
#define TILE_WIDTH 8
#define TILE_HEIGHT TILE_WIDTH

#define BUILDING_Y_SPAWN 136
#define BUILDING_Y_TILE_SPAWN BUILDING_Y_SPAWN / 8
#define MAX_JUMP_WIDTH_TILES 6
#define MIN_JUMP_WIDTH_TILES 3

#define GRAVITY (int)(1.5f * (FIX_SHIFT))
#define TERMINAL_VY GRAVITY * 20

#define GBA_WIDTH 240

#define X_SCROLL_GAIN (FIXED)(0.01f * (FIX_SCALEF))
#define X_SCROLL_MAX (FIXED)(3.5f * (FIX_SCALEF))
#define X_SCROLL_RATE 120

typedef struct scene_t {
	void (*show)(void);
	void (*update)(void);
	void (*hide)(void);
} scene_t;

extern const scene_t main_game, title_screen;

extern FIXED _scroll_x;
extern FIXED _score;

void scene_set(scene_t scene);
void scene_update();

int frame_count();
void common_step();

void init_seed(int seed);
int gba_rand();

inline int gba_rand_range(int min, int max) {
	return (gba_rand() % (max - min + 1)) + min;
}

void add_score(FIXED x);

inline void set_score(FIXED x) {
	_score = x;
}

inline FIXED get_score() {
	return _score;
}

#endif