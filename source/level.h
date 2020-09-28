#ifndef LEVEL_H

#include <tonc.h>

#define LEVEL_SIZE LEVEL_WIDTH * LEVEL_HEIGHT
#define LEVEL_HEIGHT 32
#define LEVEL_WIDTH 64

extern u16 _level[LEVEL_SIZE];

inline u16 at_level(int x, int y) {
	return _level[LEVEL_HEIGHT * x + y];
}

inline void set_level_at(int x, int y, u16 val) {
	_level[LEVEL_HEIGHT * x + y] = val;
}

inline int level_wrap_x(int x) {
	if(x >= LEVEL_WIDTH) {
		return x - LEVEL_WIDTH;
	}

	if(x < 0) {
		return x + LEVEL_WIDTH;
	}

	return x;
}

bool col_cleared(int x);

void set_level_col(int x, int y, u16 tile);

#endif