#ifndef COMMON_H
#include <tonc.h>

#define DEBUG 1

#define SB_SIZE sizeof(SCREENBLOCK)
#define BUILDING_Y_SPAWN 136
#define BUILDING_Y_TILE_SPAWN BUILDING_Y_SPAWN / 8
#define MAX_JUMP_WIDTH_TILES 5

#define LEVEL_SIZE LEVEL_WIDTH * LEVEL_HEIGHT
#define LEVEL_HEIGHT 32
#define LEVEL_WIDTH 64

#define CELL_EMPTY 	0
#define CELL_GROUND 1

static const int GRAVITY = (int)(1.5f * (FIX_SHIFT));

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