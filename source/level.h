#ifndef LEVEL_H
#define LEVEL_H

#include <tonc_types.h>

#define LEVEL_HEIGHT 32
#define LEVEL_WIDTH 64
#define LEVEL_SIZE (LEVEL_WIDTH * LEVEL_HEIGHT)

typedef enum level_col_t
{
	LEVEL_COL_EMPTY = 0,
	LEVEL_COL_GROUND = 1,
	LEVEL_LAVA = 2,
} level_col_t;

extern u16 _level[LEVEL_SIZE];
extern FIXED _bg_pos_x;

void init_level();

inline u16 at_level(int x, int y)
{
	return _level[LEVEL_HEIGHT * x + y];
}

inline void set_level_at(int x, int y, u16 val)
{
	_level[LEVEL_HEIGHT * x + y] = val;
}

inline int level_wrap_x(int x)
{
	if (x >= LEVEL_WIDTH)
	{
		return x - LEVEL_WIDTH;
	}

	if (x < 0)
	{
		return x + LEVEL_WIDTH;
	}

	return x;
}

bool col_cleared(int x);

void set_level_col(int x, int y, u16 tile);

int level_collision_rect(int x, int y, int w, int h);

int tile_to_collision(u16 tile);

void add_tile_angle(int tid, FIXED angle);

#endif