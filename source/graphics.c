#include "graphics.h"

#include <tonc.h>
#include <string.h>

#include "common.h"
#include "debug.h"

static byte _obj_tile_allc[OBJ_TILE_ALLC_SIZE];
static byte _bg_tile_allc[BG_TILE_ALLC_SIZE];

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

static int allocate_tile_idx(byte *allc, int len, int size)
{
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
			}
			return i * 2;
		}
		i += size;
	}

#ifdef DEBUG
	char str[200];
	sprintf(str, "FAILLED TO GRAPHIC ALLOCATE s:%d l:%d", size, len);
	write_to_log(LOG_LEVEL_DEBUG, str);

	str[0] = '\0';
	for (int i = 0; i < len; i++)
	{
		char b[50];
		sprintf(b, "%d, ", allc[i]);
		strcat(str, b);
	}

	write_to_log(LOG_LEVEL_DEBUG, str);
#endif
	return -1;
}

void free_tile_idx(byte *allc, int len, int idx, int size)
{
	//remeber that the idx returned from alloc is x2
	idx /= 2;
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