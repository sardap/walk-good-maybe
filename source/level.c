#include "level.h"

#include "graphics.h"
#include "common.h"
#include "debug.h"

#include "gun.h"

u16 _level[LEVEL_SIZE] = {};

void set_level_col(int x, int y, u16 tile)
{
	for (; y < LEVEL_HEIGHT; y++)
	{
		set_level_at(x, y, tile);
	}
}

bool col_cleared(int x)
{
	for (int y = 0; y < LEVEL_HEIGHT; y++)
	{
		if (at_level(x, y))
		{
			return false;
		}
	}

	return true;
}

int tile_to_collision(u16 tile)
{
	if (tile < get_buildings_tile_offset())
	{
		return LEVEL_COL_EMPTY;
	}

	return LEVEL_COL_GROUND;
}

int level_collision_rect(int x, int y, int w, int h)
{
	int tile_left = x / TILE_WIDTH;
	int tile_right = tile_left + w / TILE_WIDTH;
	int tile_top = y / TILE_WIDTH;
	int tile_bot = tile_top + h / TILE_WIDTH;

	int res = 0;

	for (int i = tile_left; i <= tile_right; i++)
	{
		for (int j = tile_top; j <= tile_bot; j++)
		{
			res |= tile_to_collision(at_level(level_wrap_x(i), j));
		}
	}

	return res;
}