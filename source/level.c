#include "level.h"

u16 _level[LEVEL_SIZE] = {};

void set_level_col(int x, int y, u16 tile) {
	for(; y < LEVEL_HEIGHT; y++) {
		set_level_at(x, y, tile);
	}
}

bool col_cleared(int x) {
	for(int y = 0; y < LEVEL_HEIGHT; y++) {
		if(at_level(x, y)) {
			return false;
		}
	}

	return true;
}