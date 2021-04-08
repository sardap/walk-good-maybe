#include "gen.h"

#include "level.h"
#include "common.h"
#include "graphics.h"
#include "debug.h"
#include "ent.h"
#include "enemy.h"
#include "obstacles.h"

#include "assets/lava0TileSet.h"
#include "assets/building0TileSet.h"
#include "assets/building1TileSet.h"
#include "assets/building2TileSet.h"
#include "assets/building3TileSet.h"
#include "assets/building4TileSet.h"
#include "assets/building5TileSet.h"
#include "assets/building6TileSet.h"

#include "assets/mgBeachIsland00.h"

//Maybe do Probability math here
//enemy_chance, lava_chance, token_chance
static t_spawn_info _building_0 = {80, 20, 70};
static t_spawn_info _building_1 = {80, 20, 70};
static t_spawn_info _building_2 = {80, 20, 70};
static t_spawn_info _building_3 = {80, 20, 70};
static t_spawn_info _building_4 = {80, 20, 70};
static t_spawn_info _building_5 = {0, 0, 100};

static t_spawn_info _island_00 = {80, 0, 70};

static int _lava_0_idx;
static int _building_0_idx;
static int _building_1_idx;
static int _building_2_idx;
static int _building_3_idx;
static int _building_4_idx;
static int _building_5_idx;
static int _building_6_idx;

static int _island_0_idx;

static int _lowest_tile_idx;
static int _highest_tile_idx;

void init_gen()
{
	_lava_0_idx = 0;
	_building_0_idx = 0;
	_building_1_idx = 0;
	_building_2_idx = 0;
	_building_3_idx = 0;
	_building_4_idx = 0;
	_building_5_idx = 0;
	_building_6_idx = 0;

	_lowest_tile_idx = 21000000;
	_highest_tile_idx = -21000000;
}

int get_lava_tile_offset()
{
	return _lava_0_idx / 2;
}

int get_buildings_tile_offset()
{
	return _lowest_tile_idx;
}

int get_buildings_tile_offset_end()
{
	//Issue this is fucked need to track which building is loaded first
	return _highest_tile_idx;
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

static bool spawn_enemy_biscuit(int start_x, int width, int y)
{
	FIXED att_x = level_to_screen(start_x + width / 2);

	create_enemy_biscut(allocate_ent(), int2fx(att_x), int2fx(y * 8 - 32));

	return true;
}

static bool spawn_enemy_biscut_ufo(int start_x, int width, int y)
{
	FIXED att_x = level_to_screen(start_x + width / 2);

	create_enemy_ufo_bisuct(
		allocate_ent(),
		int2fx(att_x), int2fx(y * 8 - 45));

	return true;
}

static bool spawn_speed_up_token(int start_x, int width, int y)
{
	FIXED att_x = level_to_screen(start_x) + gba_rand_range(0, width * 8);

	create_speed_up(allocate_ent(), int2fx(att_x), int2fx(y * 8 - 10));

	return true;
}

static bool spawn_health_up_token(int start_x, int width, int y)
{
	FIXED att_x = level_to_screen(start_x) + gba_rand_range(0, width * 8);

	create_health_up(allocate_ent(), int2fx(att_x), int2fx(y * 8 - 10));

	return true;
}

static bool spawn_jump_up_token(int start_x, int width, int y)
{
	FIXED att_x = level_to_screen(start_x) + gba_rand_range(0, width * 8);

	create_jump_up(allocate_ent(), int2fx(att_x), int2fx(y * 8 - 10));

	return true;
}

static bool spawn_shrink_token(int start_x, int width, int y)
{
	FIXED att_x = level_to_screen(start_x) + gba_rand_range(0, width * 8);

	create_shrink_token(allocate_ent(), int2fx(att_x), int2fx(y * 8 - 6));

	return true;
}

static void spawn_obstacles(int start_x, int width, int y, t_spawn_info *info)
{
	//early return to avoid that awesome wraping bug
	if (level_to_screen(start_x + width) < 0)
		return;

	if (width > 3 && gba_rand_range(1, 100) > 100 - info->lava_chance)
	{
		spawn_lava(width, start_x, y);
	}
	else if (gba_rand_range(1, 100) > 100 - info->enemy_chance)
	{
		if (gba_rand() % 2 == 0)
			spawn_enemy_biscut_ufo(start_x, width, y);
		else
			spawn_enemy_biscuit(start_x, width, y);
	}
	else if (gba_rand_range(1, 100) > 100 - info->token_chance)
	{
		switch (gba_rand_range(1, 4))
		{
		case 1:
			spawn_jump_up_token(start_x, width, y);
			break;
		case 2:
			spawn_speed_up_token(start_x, width, y);
			break;
		case 3:
			spawn_health_up_token(start_x, width, y);
			break;
		case 4:
			spawn_shrink_token(start_x, width, y);
			break;
		}
	}
}

static void update_high_low_tile(int x, int count)
{
	x /= 2;

	char str[50];
	if (x < _lowest_tile_idx)
	{
		_lowest_tile_idx = x;
		sprintf(str, "NL:%d", x);
		write_to_log(LOG_LEVEL_DEBUG, str);
	}

	if (x + count > _highest_tile_idx)
	{
		_highest_tile_idx = x + count;
		sprintf(str, "NM:%d", x);
		write_to_log(LOG_LEVEL_DEBUG, str);
	}
}

void load_building_0(int cb)
{
	_building_0_idx = allocate_bg_tile_idx(building0TileSetTilesLen / 64 - 1);
	dma3_cpy(&tile_mem[cb][_building_0_idx], building0TileSetTiles + 32, building0TileSetTilesLen - 64);

	update_high_low_tile(_building_0_idx, TILES_COUNT(building0TileSetTilesLen) - 1);
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

	spawn_obstacles(start_x, width, y, &_building_0);

	return width;
}

void load_building_1(int cb)
{
	_building_1_idx = allocate_bg_tile_idx(building1TileSetTilesLen / 64 - 1);
	dma3_cpy(&tile_mem[cb][_building_1_idx], building1TileSetTiles + 32, building1TileSetTilesLen - 64);

	update_high_low_tile(_building_1_idx, TILES_COUNT(building1TileSetTilesLen) - 1);
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

	spawn_obstacles(start_x, width, y, &_building_1);

	return width;
}

void load_building_2(int cb)
{
	_building_2_idx = allocate_bg_tile_idx(building2TileSetTilesLen / 64 - 1);
	dma3_cpy(&tile_mem[cb][_building_2_idx], building2TileSetTiles + 32, building2TileSetTilesLen - 64);

	update_high_low_tile(_building_2_idx, TILES_COUNT(building3TileSetTilesLen) - 1);
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

	spawn_obstacles(start_x, width, y, &_building_2);

	return width;
}

void load_building_3(int cb)
{
	_building_3_idx = allocate_bg_tile_idx(building3TileSetTilesLen / 64 - 1);
	dma3_cpy(&tile_mem[cb][_building_3_idx], building3TileSetTiles + 32, building3TileSetTilesLen - 64);

	update_high_low_tile(_building_3_idx, TILES_COUNT(building3TileSetTilesLen) - 1);
}

void unload_building_3()
{
	free_bg_tile_idx(_building_3_idx, building3TileSetTilesLen / 64 - 1);
}

int spawn_building_3(int start_x)
{
	int x_base = start_x;
	int x;
	int y = gba_rand_range(CITY_BUILDING_Y_TILE_SPAWN - 3, CITY_BUILDING_Y_TILE_SPAWN);

	int tile = _building_3_idx / 2;
	//LEFT SECTION
	set_level_at(x_base, y, BUILDING_3_ROOF_TOP + tile);
	set_level_at(x_base, y + 1, BUILDING_3_ROOF_LEFT + tile);
	set_level_col(x_base, y + 2, BUILDING_3_LEFT + tile);

	int width = gba_rand_range(2, 4);

	//MIDDLE SECTION
	for (int i = 1; i < width; i++)
	{
		x = level_wrap_x(x_base + i);
		set_level_at(x, y, BUILDING_3_ROOF_TOP + tile);
		set_level_at(x, y + 1, BUILDING_3_ROOF_MIDDLE + tile);
		for (int j = y + 2; j < 30; j += 2)
		{
			set_level_at(x, j, BUILDING_3_MIDDLE_WINDOW + tile);
			set_level_at(x, j + 1, BUILDING_3_MIDDLE_WHITE + tile);
		}
	}

	//RIGHT SECTION
	x = level_wrap_x(x_base + width);
	set_level_at(x, y, BUILDING_3_ROOF_TOP + tile);
	set_level_at(x, y + 1, BUILDING_3_ROOF_RIGHT + tile);
	set_level_col(x, y + 2, BUILDING_3_RIGHT + tile);

	spawn_obstacles(start_x, width, y, &_building_3);

	return width;
}

void load_building_4(int cb)
{
	_building_4_idx = allocate_bg_tile_idx(building4TileSetTilesLen / 64 - 1);
	dma3_cpy(&tile_mem[cb][_building_4_idx], building4TileSetTiles + 32, building4TileSetTilesLen - 64);

	update_high_low_tile(_building_4_idx, TILES_COUNT(building4TileSetTilesLen) - 1);
}

void unload_building_4()
{
	free_bg_tile_idx(_building_4_idx, building4TileSetTilesLen / 64 - 1);
}

int spawn_building_4(int start_x)
{
	int x_base = start_x;
	int x;
	int y = gba_rand_range(CITY_BUILDING_Y_TILE_SPAWN - 5, CITY_BUILDING_Y_TILE_SPAWN - 3);

	int tile = _building_4_idx / 2;
	//LEFT SECTION
	set_level_at(x_base, y, BUILDING_4_ROOF_LEFT + tile);
	set_level_col(x_base, y + 1, BUILDING_4_LEFT_BOT + tile);

	int width = gba_rand_range(5, 7);

	//MIDDLE SECTION
	for (int i = 1; i < width; ++i)
	{
		x = level_wrap_x(x_base + i);
		set_level_at(x, y, BUILDING_4_ROOF_MIDDLE + tile);

		for (int j = y + 1; j < 30; j += 2)
		{
			if (i == 1 || i == width - 1)
			{
				switch (gba_rand() % 6)
				{
				case 1:
					set_level_at(x, j, BUILDING_4_SIGN_YELLOW_TOP + tile);
					set_level_at(x, j + 1, BUILDING_4_SIGN_YELLOW_BOT + tile);
					break;
				case 2:
					set_level_at(x, j, BUILDING_4_SIGN_ORANGE_TOP + tile);
					set_level_at(x, j + 1, BUILDING_4_SIGN_ORANGE_BOT + tile);
					break;
				case 3:
					set_level_at(x, j, BUILDING_4_SIGN_GREEN_TOP + tile);
					set_level_at(x, j + 1, BUILDING_4_SIGN_GREEN_BOT + tile);
					break;
				case 4:
				{
					if (gba_rand() % 2 == 0)
					{
						set_level_at(x, j, BUILDING_4_SIGN_RED + tile);
						set_level_at(x, j + 1, BUILDING_4_SIGN_BLUE + tile);
					}
					else
					{
						set_level_at(x, j, BUILDING_4_SIGN_RED + tile);
						set_level_at(x, j + 1, BUILDING_4_MIDDLE_BLANK + tile);
					}
					break;
				}
				case 5:
				{
					if (gba_rand() % 2 == 0)
					{
						set_level_at(x, j, BUILDING_4_SIGN_BLUE + tile);
						set_level_at(x, j + 1, BUILDING_4_SIGN_RED + tile);
					}
					else
					{
						set_level_at(x, j, BUILDING_4_SIGN_BLUE + tile);
						set_level_at(x, j + 1, BUILDING_4_MIDDLE_BLANK + tile);
					}
					break;
				}
				default:
					set_level_at(x, j, BUILDING_4_MIDDLE_BLANK + tile);
					set_level_at(x, j + 1, BUILDING_4_MIDDLE_BLANK + tile);
					break;
				}
			}
			else
			{
				set_level_at(x, j, BUILDING_4_MIDDLE_WINDOW + tile);
				set_level_at(x, j + 1, BUILDING_4_MIDDLE_BLANK + tile);
			}
		}
	}

	//RIGHT SECTION
	x = level_wrap_x(x_base + width);
	set_level_at(x, y, BUILDING_4_ROOF_RIGHT + tile);
	set_level_col(x, y + 1, BUILDING_4_RIGHT_BOT + tile);

	spawn_obstacles(start_x, width, y, &_building_4);

	return width;
}

void load_building_5(int cb)
{
	_building_5_idx = allocate_bg_tile_idx(building5TileSetTilesLen / 64 - 1);
	dma3_cpy(&tile_mem[cb][_building_5_idx], building5TileSetTiles + 32, building5TileSetTilesLen - 64);

	update_high_low_tile(_building_5_idx, TILES_COUNT(building5TileSetTilesLen) - 1);
}

void free_building_5()
{
	free_bg_tile_idx(_building_5_idx, building5TileSetTilesLen / 64 - 1);
}

int spawn_building_5(int start_x)
{
	int x_base = start_x;
	int x;
	int y = gba_rand_range(CITY_BUILDING_Y_TILE_SPAWN - 4, CITY_BUILDING_Y_TILE_SPAWN - 3);

	int tile = _building_5_idx / 2;
	//LEFT SECTION
	set_level_at(x_base, y, BUILDING_5_ROOF_LEFT + tile);
	set_level_col(x_base, y + 1, BUILDING_5_MIDDLE_LEFT + tile);

	set_level_at(level_wrap_x(x_base + 1), y, BUILDING_5_ROOF_MIDDLE + tile);
	set_level_col(level_wrap_x(x_base + 1), y + 1, BUILDING_5_MIDDLE_WINDOW + tile);

	int width = 5;
	int y_start = y;

	//MIDDLE SECTION
	for (int i = 2; i < width; ++i)
	{
		x = level_wrap_x(x_base + i);
		int roof = i + 4 > width ? BUILDING_5_ROOF_RIGHT : BUILDING_5_ROOF_MIDDLE;
		set_level_at(x, y, roof + tile);
		set_level_col(x, y + 1, BUILDING_5_MIDDLE_WINDOW + tile);

		if (i + 4 > width)
		{
			y = clamp(y + 1, 0, CITY_BUILDING_Y_TILE_SPAWN + 1);
		}
	}

	//RIGHT SECTION
	x = level_wrap_x(x_base + width);
	set_level_at(x, y, BUILDING_5_ROOF_RIGHT + tile);
	set_level_col(x, y + 1, BUILDING_5_MIDDLE_RIGHT + tile);

	spawn_obstacles(start_x, 1, y_start, &_building_5);

	return width;
}

void load_building_6(int cb)
{
	_building_6_idx = allocate_bg_tile_idx(building6TileSetTilesLen / 64 - 1);
	dma3_cpy(&tile_mem[cb][_building_6_idx], building6TileSetTiles + 32, building6TileSetTilesLen - 64);

	update_high_low_tile(_building_6_idx, TILES_COUNT(building6TileSetTilesLen) - 1);
}

void free_building_6()
{
	free_bg_tile_idx(_building_6_idx, building6TileSetTilesLen / 64 - 1);
}

int spawn_building_6(int start_x)
{
	int x_base = start_x;
	int x;
	int y = CITY_BUILDING_Y_TILE_SPAWN;

	int tile = _building_6_idx / 2;
	//LEFT SECTION
	set_level_at(x_base, y, BUILDING_6_BLANK + tile);
	set_level_col(x_base, y + 1, BUILDING_6_BLANK + tile);

	int width = 5;

	//MIDDLE SECTION
	for (int i = 1; i < width; ++i)
	{
		x = level_wrap_x(x_base + i);
		set_level_at(x, y, BUILDING_6_BLANK + tile);
		set_level_col(x, y + 1, BUILDING_6_BLANK + tile);
	}

	//RIGHT SECTION
	x = level_wrap_x(x_base + width);
	set_level_at(x, y, BUILDING_6_ANGLE_LEFT + tile);
	set_level_col(x, y + 1, BUILDING_6_BLANK + tile);

	return width;
}

void load_island_00(int cb)
{
	_island_0_idx = allocate_bg_tile_idx(TILE_COUNT(mgBeachIsland00TilesLen) - 1);
	dma3_cpy(&tile_mem[cb][_island_0_idx], mgBeachIsland00Tiles + 32, mgBeachIsland00TilesLen - 64);

	update_high_low_tile(_island_0_idx, TILE_COUNT(mgBeachIsland00TilesLen) - 1);
}

void free_island_00()
{
	free_bg_tile_idx(_island_0_idx, TILE_COUNT(mgBeachIsland00TilesLen) - 1);
}

int spawn_island_00(int start_x)
{
	int x_base = start_x;
	int y = BEACH_ISLAND_Y_TILE_SPAWN;

	int tile = _island_0_idx / 2;

	int width = gba_rand_range(6, 10);
	set_level_at(level_wrap_x(x_base), y, ISLAND_00_LEFT + tile);

	//MIDDLE SECTION
	for (int i = 1; i < width - 1; ++i)
	{
		set_level_at(level_wrap_x(x_base + i), y, ISLAND_00_TOP + tile);
	}

	set_level_at(level_wrap_x(x_base + width - 1), y, ISLAND_00_RIGHT + tile);

	spawn_obstacles(start_x, width, y, &_island_00);

	return width;
}
