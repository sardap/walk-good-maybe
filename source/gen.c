#include "gen.h"

#include "level.h"
#include "common.h"
#include "graphics.h"
#include "debug.h"
#include "ent.h"
#include "enemy.h"

#include "assets/lava0TileSet.h"
#include "assets/building0TileSet.h"
#include "assets/building1TileSet.h"
#include "assets/building2TileSet.h"

int _lava_0_idx;
int _building_0_idx;
int _building_1_idx;
int _building_2_idx;

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
	return (_building_2_idx / 2) + building2TileSetTilesLen / 64 - 1;
}

void load_lava_0(int cb)
{
	_lava_0_idx = allocate_bg_tile_idx(lava0TileSetTilesLen / 64 - 1);
	//These tilesets are fucked and we need a buffer at the start which we don't need to copy
	dma3_cpy(&tile_mem[cb][_lava_0_idx], lava0TileSetTiles + 32, lava0TileSetTilesLen - 64);
}

void unload_lava_0()
{
	free_bg_tile_idx(_lava_0_idx, lava0TileSetTilesLen / 64 - 1);
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

static FIXED level_to_screen(int level_x)
{
	//I can't do this at the moment
	if (fx2int(_bg_pos_x) + GBA_WIDTH >= 64 * 8)
		return -1;

	return (level_x * 8) - fx2int(_bg_pos_x);
}

void load_building_0(int cb)
{
	_building_0_idx = allocate_bg_tile_idx(building0TileSetTilesLen / 64 - 1);
	dma3_cpy(&tile_mem[cb][_building_0_idx], building0TileSetTiles + 32, building0TileSetTilesLen - 64);
}

void unload_building_0()
{
	free_bg_tile_idx(_building_0_idx, building0TileSetTilesLen / 64 - 1);
}

int spawn_building_0(int start_x)
{
	int x_base = start_x;
	int x;
	int y = gba_rand_range(CITY_BUILDING_Y_TILE_SPAWN - 3, CITY_BUILDING_Y_TILE_SPAWN);

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

	//RIGHT SECTION
	x = level_wrap_x(x_base + width);
	set_level_at(x, y, BUILDING_0_BRICK + tile);
	set_level_col(x, y, BUILDING_0_BRICK + tile);

	if (width > 3 && gba_rand() % 4 == 0)
	{
		spawn_lava(width, x_base, y);
		return width;
	}

	FIXED att_x = level_to_screen(start_x + width / 2);
	//early return to avoid that awesome wraping bug
	if (att_x < 0 || gba_rand() % 100 > 75)
		return width;

	int ent_idx = allocate_att(1);
	create_toast_enemy(
		&_ents[ent_idx], ent_idx,
		int2fx(att_x), int2fx(y * 8 - 32));

	return width;
}

void load_building_1(int cb)
{
	_building_1_idx = allocate_bg_tile_idx(building1TileSetTilesLen / 64 - 1);
	dma3_cpy(&tile_mem[cb][_building_1_idx], building1TileSetTiles + 32, building1TileSetTilesLen - 64);
}

void unload_building_1()
{
	free_bg_tile_idx(_building_1_idx, building1TileSetTilesLen / 64 - 1);
}

int spawn_building_1(int start_x)
{
	int x_base = start_x;
	int x;
	int y = CITY_BUILDING_Y_TILE_SPAWN;

	int tile = _building_1_idx / 2;
	//LEFT SECTION
	set_level_at(x_base, y, BUILDING_1_LEFT_ROOF + tile);
	set_level_col(x_base, y + 1, BUILDING_1_LEFT_BOT + tile);

	int width = gba_rand_range(3, 5);
	//MIDDLE SECTION
	for (int i = 1; i < width; i++)
	{
		x = level_wrap_x(x_base + i);
		set_level_at(x, y, BUILDING_1_MIDDLE_ROOF + tile);
		set_level_col(x, y + 1, BUILDING_1_MIDDLE_BOT + tile);
	}

	//RIGHT SECTION
	x = level_wrap_x(x_base + width);
	set_level_at(x, y, BUILDING_1_RIGHT_ROOF + tile);
	set_level_col(x, y + 1, BUILDING_1_RIGHT_BOT + tile);

	if (width > 3 && gba_rand() % 4 == 0)
	{
		spawn_lava(width, x_base, y);
		return width;
	}

	FIXED att_x = level_to_screen(start_x + width / 2);
	//early return to avoid that awesome wraping bug
	if (att_x < 0 || gba_rand() % 100 > 75)
		return width;

	int ent_idx = allocate_att(1);
	create_toast_enemy(
		&_ents[ent_idx], ent_idx,
		int2fx(att_x), int2fx(y * 8 - 32));

	return width;
}

void load_building_2(int cb)
{
	_building_2_idx = allocate_bg_tile_idx(building2TileSetTilesLen / 64 - 1);
	dma3_cpy(&tile_mem[cb][_building_2_idx], building2TileSetTiles + 32, building2TileSetTilesLen - 64);
}

void unload_building_2()
{
	free_bg_tile_idx(_building_2_idx, building2TileSetTilesLen / 64 - 1);
}

int spawn_building_2(int start_x)
{
	int x_base = start_x;
	int x;
	int y = gba_rand_range(CITY_BUILDING_Y_TILE_SPAWN - 5, CITY_BUILDING_Y_TILE_SPAWN);

	int tile = _building_2_idx / 2;
	//LEFT SECTION
	set_level_at(x_base, y, BUILDING_2_LEFT_ROOF + tile);
	set_level_col(x_base, y + 1, BUILDING_2_LEFT_MIDDLE + tile);

	int width = gba_rand_range(3, 8);
	if (width % 2 != 0)
		width++;

	//MIDDLE SECTION
	for (int i = 1; i < width; i++)
	{
		x = level_wrap_x(x_base + i);
		set_level_at(x, y, BUILDING_2_MIDDLE_ROOF_HANGING + tile);
		for (int j = y + 1; j < 30; j += 3)
		{
			if (i % 2 == 0)
			{
				set_level_at(x, j, BUILDING_2_WINDOW + tile);
				set_level_at(x, j + 1, BUILDING_2_WINDOW + tile);
			}
			else
			{
				set_level_at(x, j, BUILDING_2_MIDDLE_WHITE + tile);
				set_level_at(x, j + 1, BUILDING_2_MIDDLE_WHITE_CLEAN + tile);
			}

			set_level_at(x, j + 2, BUILDING_2_MIDDLE_BLUE + tile);
		}
	}

	//RIGHT SECTION
	x = level_wrap_x(x_base + width);
	set_level_at(x, y, BUILDING_2_RIGHT_ROOF + tile);
	set_level_col(x, y + 1, BUILDING_2_RIGHT_MIDDLE + tile);

	if (width > 3 && gba_rand() % 6 == 0)
	{
		spawn_lava(width, x_base, y);
		return width;
	}

	FIXED att_x = level_to_screen(start_x + width / 2);
	//early return to avoid that awesome wraping bug
	if (att_x < 0 || gba_rand() % 100 > 75)
		return width;

	int ent_idx = allocate_att(1);
	create_toast_enemy(
		&_ents[ent_idx], ent_idx,
		int2fx(att_x), int2fx(y * 8 - 32));

	return width;
}
