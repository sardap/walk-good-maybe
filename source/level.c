#include "level.h"

#include "gen.h"
#include "common.h"
#include "debug.h"

#include "gun.h"

u16 _level[LEVEL_SIZE] = {};
FIXED _bg_pos_x;

void init_level()
{
	_bg_pos_x = 0;

	for (int i = 0; i < LEVEL_SIZE; i++)
	{
		_level[i] = 0;
	}
}

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

//Slow point
int tile_to_collision(u16 tile)
{
	if (tile >= get_lava_tile_offset() && tile <= get_lava_tile_offset() + LAVA_TILE_END)
	{
		return LEVEL_LAVA;
	}

	if (tile >= get_buildings_tile_offset() && tile <= get_buildings_tile_offset_end())
	{
		return LEVEL_COL_GROUND;
	}

	return LEVEL_COL_EMPTY;
}

int level_collision_rect(int x, int y, int w, int h)
{
	int tile_left = x / TILE_WIDTH;
	int tile_right = tile_left + INT_DIV_CEIL(w, TILE_WIDTH);
	int tile_top = y / TILE_WIDTH;
	int tile_bot = tile_top + INT_DIV_CEIL(h, TILE_WIDTH);

	int result = 0;
	for (int i = tile_left; i <= tile_right; i++)
	{
		for (int j = tile_top; j <= tile_bot; j++)
		{
			result |= tile_to_collision(at_level(level_wrap_x(i), j));
		}
	}

	return result;
}