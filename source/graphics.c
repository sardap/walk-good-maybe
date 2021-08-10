#include "graphics.h"

#include <tonc.h>
#include <string.h>
#include <limits.h>

#include "common.h"
#include "debug.h"

const u16 _water_cycle[] = {0x7FFF, 0x7FC6, 0x7B80, 0x7FD0, 0x7FB2, 0x7FD7};

// Tiles
static byte _obj_tile_allc[OBJ_TILE_ALLC_SIZE];
static byte _bg_tile_allc[BG_TILE_ALLC_SIZE];
// Pallettes
static byte _obj_pal_allc[OBJ_PAL_ALLC_SIZE];

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

static int allocate_idx(byte *allc, int len, int size)
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

void free_idx(byte *allc, int len, int idx, int size)
{
	// remeber that the idx returned from alloc is x2
	idx = CLAMP(idx / 2, 0, INT_MAX);
	for (int i = idx; i < idx + size; i++)
	{
		allc[i] = 0;
	}
}

int allocate_obj_tile_idx(int size)
{
	int result = allocate_idx(_obj_tile_allc, OBJ_TILE_ALLC_SIZE, size);
#ifdef DEBUG
	if (result < 0)
	{
		char str[50];
		sprintf(str, "FAILLED TO ALLOCATE OBJ TILE %d", size);
		write_to_log(LOG_LEVEL_DEBUG, str);
	}
#endif
	return result;
}

void free_obj_tile_idx(int idx, int size)
{
	free_idx(_obj_tile_allc, OBJ_TILE_ALLC_SIZE, idx, size);
}

int allocate_bg_tile_idx(int size)
{
	int result = allocate_idx(_bg_tile_allc, BG_TILE_ALLC_SIZE, size);
#ifdef DEBUG
	if (result < 0)
	{
		char str[50];
		sprintf(str, "FAILLED TO ALLOCATE BG TILE %d", size);
		write_to_log(LOG_LEVEL_DEBUG, str);
	}
#endif
	return result;
}

void free_bg_tile_idx(int idx, int size)
{
	free_idx(_bg_tile_allc, BG_TILE_ALLC_SIZE, idx, size);
}

int allocate_obj_pal_idx(int size)
{
	int result = allocate_idx(_obj_pal_allc, OBJ_PAL_ALLC_SIZE, size) / 2;
#ifdef DEBUG
	if (result < 0)
	{
		char str[50];
		sprintf(str, "FAILLED TO ALLOCATE OBJ TILE %d", size);
		write_to_log(LOG_LEVEL_DEBUG, str);
	}
#endif
	return result;
}

void free_obj_pal_idx(int idx, int size)
{
	free_idx(_obj_pal_allc, OBJ_PAL_ALLC_SIZE, idx / 2, size);
}
