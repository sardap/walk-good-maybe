#include "ui_display.h"

#include "ent.h"
#include "common.h"
#include "graphics.h"
#include "player.h"
#include "debug.h"

#include "assets/lifeAmmount3.h"
#include "assets/lifeAmmount2.h"
#include "assets/lifeAmmount1.h"
#include "assets/speedLevel0.h"
#include "assets/speedLevel1.h"
#include "assets/speedLevel2.h"
#include "assets/speedLevel3.h"
#include "assets/speedLevel4.h"

static const u32 *_speed_level_tiles[] = {
	speedLevel4Tiles, speedLevel3Tiles,
	speedLevel2Tiles, speedLevel1Tiles,
	speedLevel0Tiles};
static const int _speed_level_tiles_length = 5;

static int _life_tile_start_idx;
static int _v_ent_life_idx;
static int _speed_level_idx;
static int _v_ent_speed_level;

void load_life_display()
{
	_life_tile_start_idx = allocate_obj_tile_idx(4);
	update_life_display(PLAYER_LIFE_START);

	_v_ent_life_idx = allocate_visual_ent(1);

	visual_ent_t *ent = &_visual_ents[_v_ent_life_idx];

	ent->type = TYPE_VISUAL_LIFE;
	ent->ent_idx = _v_ent_life_idx;

	ent->x = (GBA_WIDTH - 16) * FIX_SCALE;
	ent->y = 0;

	ent->att.attr0 = ATTR0_SQUARE | ATTR0_8BPP;
	ent->att.attr1 = ATTR1_SIZE_16x16;
	ent->att.attr2 = ATTR2_PALBANK(0) | ATTR2_PRIO(0) | ATTR2_ID(_life_tile_start_idx);
}

void update_life_display(int life)
{
	u32 *tiles = (u32 *)lifeAmmount1Tiles;
	switch (life)
	{
	case 3:
		tiles = (u32 *)lifeAmmount3Tiles;
		break;
	case 2:
		tiles = (u32 *)lifeAmmount2Tiles;
		break;
	}
	dma3_cpy(&tile_mem[4][_life_tile_start_idx], tiles, lifeAmmount3TilesLen);
}

void unload_life_display()
{
	free_obj_tile_idx(_life_tile_start_idx, 4);
	free_visual_ent(_v_ent_life_idx, 1);
}

void load_speed_level_display()
{
	_speed_level_idx = allocate_obj_tile_idx(4);
	update_speed_level_display(PLAYER_AIR_START_SLOWDOWN);

	_v_ent_speed_level = allocate_visual_ent(1);

	visual_ent_t *ent = &_visual_ents[_v_ent_speed_level];

	ent->type = TYPE_VISUAL_SPEED_LEVEL;
	ent->ent_idx = _v_ent_speed_level;

	ent->x = (GBA_WIDTH - 32) * FIX_SCALE;
	ent->y = 0;

	ent->att.attr0 = ATTR0_SQUARE | ATTR0_8BPP;
	ent->att.attr1 = ATTR1_SIZE_16x16;
	ent->att.attr2 = ATTR2_PALBANK(0) | ATTR2_PRIO(0) | ATTR2_ID(_speed_level_idx);
}

void update_speed_level_display(FIXED speed)
{
	FIXED inc = fxdiv(PLAYER_AIR_START_SLOWDOWN - PLAYER_AIR_SLOWDOWN_MIN, int2fx(_speed_level_tiles_length));

	for (int i = 0; i < _speed_level_tiles_length; i++)
	{
		if (speed <= PLAYER_AIR_SLOWDOWN_MIN + inc * i)
		{
			dma3_cpy(&tile_mem[4][_speed_level_idx], _speed_level_tiles[i], speedLevel0TilesLen);
			return;
		}
	}

	dma3_cpy(&tile_mem[4][_speed_level_idx], _speed_level_tiles[_speed_level_tiles_length - 1], speedLevel0TilesLen);
}
