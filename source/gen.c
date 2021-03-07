#include "gen.h"

#include "level.h"
#include "common.h"
#include "graphics.h"
#include "debug.h"

#include "assets/lava0TileSet.h"
#include "assets/building0TileSet.h"
#include "assets/building1TileSet.h"

int _lava_0_idx;
int _building_0_idx;
int _building_1_idx;

int get_lava_tile_offset()
{
	return _lava_0_idx / 2;
}

int get_buildings_tile_offset()
{
	//Issue this is fucked need to track which building is loaded first
	return _building_0_idx / 2;
}

int get_buildings_tile_offset_end()
{
	//Issue this is fucked need to track which building is loaded first
	return (_building_1_idx / 2) + BUILDING_1_RIGHT_BOT;
}

void load_lava_0(int cb)
{
	_lava_0_idx = allocate_bg_tile_idx(3);
	//These tilesets are fucked and we need a buffer at the start which we don't need to copy
	dma3_cpy(&tile_mem[cb][_lava_0_idx], lava0TileSetTiles + 32, lava0TileSetTilesLen - 64);
}

void unload_lava_0()
{
	free_bg_tile_idx(_lava_0_idx, 3);
}

static void spawn_lava(int width, int x_base, int y)
{
	int lava_width = gba_rand_range(3, width - 2);
	int start = gba_rand_range(1, width - lava_width);
	for (int i = start; i < start + lava_width; i++)
	{
		int x = level_wrap_x(x_base + i);
		if (i == start)
			set_level_at(x, y, get_lava_tile_offset() + LAVA_TILE_LEFT);
		else if (i == start + lava_width - 1)
			set_level_at(x, y, get_lava_tile_offset() + LAVA_TILE_RIGHT);
		else
			set_level_at(x, y, get_lava_tile_offset() + LAVA_TILE_MIDDLE);
	}
}

void load_building_0(int cb)
{
	_building_0_idx = allocate_bg_tile_idx(2);
	dma3_cpy(&tile_mem[cb][_building_0_idx], building0TileSetTiles + 32, building0TileSetTilesLen - 64);
}

void unload_building_0()
{
	free_bg_tile_idx(_building_0_idx, 2);
}

int spawn_building_0(int start_x)
{
	int x_base = start_x;
	int x;
	int y = gba_rand_range(BUILDING_Y_TILE_SPAWN - 3, BUILDING_Y_TILE_SPAWN);

	int tile = _building_0_idx / 2;
	//LEFT SECTION
	set_level_at(x_base, y, BUILDING_0_BRICK + tile);
	set_level_col(x_base, y + 1, BUILDING_0_BRICK + tile);

	int width = gba_rand_range(5, 10);
	for (int i = 1; i < width; i++)
	{
		x = level_wrap_x(x_base + i);
		set_level_at(x, y, BUILDING_0_BRICK + tile);
		set_level_col(x, y + 1, BUILDING_0_WINDOW + tile);
	}

	if (width > 3 && gba_rand() % 2 == 0)
	{
		spawn_lava(width, x_base, y);
	}

	//RIGHT SECTION
	x = level_wrap_x(x_base + width);
	set_level_at(x, y, BUILDING_0_BRICK + tile);
	set_level_col(x, y, BUILDING_0_BRICK + tile);

	return width;
}

void load_building_1(int cb)
{
	_building_1_idx = allocate_bg_tile_idx(6);
	dma3_cpy(&tile_mem[cb][_building_1_idx], building1TileSetTiles + 32, building1TileSetTilesLen - 64);
}

void unload_building_1()
{
	free_bg_tile_idx(_building_1_idx, 6);
}

int spawn_building_1(int start_x)
{
	int x_base = start_x;
	int x;
	int y = BUILDING_Y_TILE_SPAWN;

	int tile = _building_1_idx / 2;
	//LEFT SECTION
	set_level_at(x_base, y, BUILDING_1_LEFT_ROOF + tile);
	set_level_col(x_base, y + 1, BUILDING_1_LEFT_BOT + tile);

	int width = gba_rand_range(3, 7);
	//MIDDLE SECTION
	for (int i = 1; i < width; i++)
	{
		x = level_wrap_x(x_base + i);
		set_level_at(x, y, BUILDING_1_MIDDLE_ROOF + tile);
		set_level_col(x, y + 1, BUILDING_1_MIDDLE_BOT + tile);
	}

	if (width > 3 && gba_rand() % 5 == 0)
	{
		spawn_lava(width, x_base, y);
	}

	//RIGHT SECTION
	x = level_wrap_x(x_base + width);
	set_level_at(x, y, BUILDING_1_RIGHT_ROOF + tile);
	set_level_col(x, y + 1, BUILDING_1_RIGHT_BOT + tile);

	return width;
}
