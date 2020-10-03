#ifndef COMMON_H
#define COMMON_H

#include <tonc.h>

#define SB_SIZE sizeof(SCREENBLOCK)
#define TILE_WIDTH 8
#define TILE_HEIGHT TILE_WIDTH

#define BUILDING_Y_SPAWN 136
#define BUILDING_Y_TILE_SPAWN BUILDING_Y_SPAWN / 8
#define MAX_JUMP_WIDTH_TILES 6

#define GRAVITY (int)(1.5f * (FIX_SHIFT))
#define TERMINAL_VY GRAVITY * 20

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
	return (gba_rand() % (max - min + 1)) + min;
}

#endif