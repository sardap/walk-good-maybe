#include "graphics.h"

#include <tonc.h>
#include <string.h>

#include "common.h"
#include "debug.h"

static int _obj_tile_allc[OBJ_TILE_ALLC_SIZE];
static int _bg_tile_allc[BG_TILE_ALLC_SIZE];

static int _building_offset;

void init_graphics()
{
	for (int i = 0; i < OBJ_TILE_ALLC_SIZE; i++)
	{
		_obj_tile_allc[i] = 0;
	}

	for (int i = 0; i < BG_TILE_ALLC_SIZE; i++)
	{
		_bg_tile_allc[i] = 0;
	}
}

static int allocate_tile_idx(int *allc, int len, int size)
{
	size = size * 2;
	for (int i = 0; i < len;)
	{
		bool found = true;
		for (int j = i; j - i < size && j < len; j++)
		{
			if (allc[j] > 0)
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			for (int j = 0; j < size; j++)
			{
				allc[i + j] = 1;
				write_to_log(LOG_LEVEL_INFO, "END");
			}
			return i;
		}
		i += size;
	}

	return -1;
}

void free_tile_idx(int *allc, int len, int idx, int size)
{
	size = size * 2;
	for (int i = idx; i < idx + size; i++)
	{
		allc[i] = 0;
	}
}

int allocate_obj_tile_idx(int size)
{
	return allocate_tile_idx(_obj_tile_allc, OBJ_TILE_ALLC_SIZE, size);
}

void free_obj_tile_idx(int idx, int size)
{
	free_tile_idx(_obj_tile_allc, OBJ_TILE_ALLC_SIZE, idx, size);
}

int allocate_bg_tile_idx(int size)
{
	return allocate_tile_idx(_bg_tile_allc, BG_TILE_ALLC_SIZE, size);
}

void free_bg_tile_idx(int idx, int size)
{
	free_tile_idx(_bg_tile_allc, BG_TILE_ALLC_SIZE, idx, size);
}

int get_buildings_tile_offset()
{
	return _building_offset;
}

void set_buildings_tiles_offset(int idx)
{
	_building_offset = idx;
}

static inline int calc_address(int width, int x, int y)
{
	return width * y + x;
}

bool valid_cloud_address(int tile_offset, int sb, int width, int x, int y)
{
	int address = calc_address(width, x, y);

	for (int i = 0; i < 4; i++)
	{
		if (se_mem[sb][address + i] != tile_offset)
		{
			return false;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		if (se_mem[sb][address + 32 + i] != tile_offset)
		{
			return false;
		}
	}

	return true;
}

void create_cloud(int tile_offset, int sb, int width, int x, int y)
{
	int address = calc_address(width, x, y);

	se_mem[sb][address + 0] = tile_offset + 0x1;
	se_mem[sb][address + 1] = tile_offset + 0x2;
	se_mem[sb][address + 2] = tile_offset + 0x3;
	se_mem[sb][address + 3] = tile_offset + 0x4;
	se_mem[sb][address + 32] = tile_offset + 0x5;
	se_mem[sb][address + 33] = tile_offset + 0x6;
	se_mem[sb][address + 34] = tile_offset + 0x7;
	se_mem[sb][address + 35] = tile_offset + 0x8;
}

void place_n_clouds(int tile_offset, int sb, int width, int n)
{
	for (int i = 0; i < n; i++)
	{
		int j = 0;
		while (1)
		{
			int y = gba_rand_range(0, 10);
			int x = gba_rand_range(0, 28);
			j++;
			if (valid_cloud_address(tile_offset, sb, width, x, y))
			{
				create_cloud(tile_offset, sb, width, x, y);
				j = 6;
			}

			if (j > 5)
			{
				break;
			}
		}
	}
}