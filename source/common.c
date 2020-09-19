#include "common.h"

#include <stdlib.h>

static int _frame_count;

static void nothing(void) {}

static scene_t next_scene = {
	.show = nothing,
};

static scene_t current_scene = {
	.show = nothing,
};

static bool scene_changed = false;

void common_step() {
	_frame_count++;
}

int frame_count() {
	return _frame_count;
}

void scene_set(scene_t scene) {
	scene_changed = true;
	next_scene = scene;
}

void scene_update() {
	if (scene_changed) {
		scene_changed = false;
		// current_scene.hide();
		current_scene = next_scene;
		current_scene.show();
	}
	current_scene.update();
	_frame_count++;
}


void init_seed(int seed) {
	srand((unsigned int)seed);
}

int gba_rand() {
	return rand();	
}