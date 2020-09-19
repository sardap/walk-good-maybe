#ifndef COMMON_H
#include <tonc.h>

static const int sb_size = sizeof(SCREENBLOCK);

typedef struct scene_t {
	void (*show)(void);
	void (*update)(void);
} scene_t;

extern const scene_t main_game;

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