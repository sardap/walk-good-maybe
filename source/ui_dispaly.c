#include "ui_display.h"

#include "ent.h"
#include "common.h"
#include "graphics.h"
#include "player.h"
#include "debug.h"
#include "numbers.h"

#include "assets/lifeAmmount3.h"
#include "assets/lifeAmmount2.h"
#include "assets/lifeAmmount1.h"
#include "assets/speedLevel0.h"
#include "assets/speedLevel1.h"
#include "assets/speedLevel2.h"
#include "assets/speedLevel3.h"
#include "assets/speedLevel4.h"
#include "assets/jumpLevel0.h"

static const u32 *_speed_level_tiles[] = {
	speedLevel4Tiles, speedLevel3Tiles,
	speedLevel2Tiles, speedLevel1Tiles,
	speedLevel0Tiles};
static const int _speed_level_tiles_length = 5;

static int _life_tile_start_idx;
static int _speed_level_tile_idx;
static int _jump_level_tile_idx;
static visual_ent_t *_score_digs[SCORE_DIGITS];

void load_life_display()
{
	_life_tile_start_idx = allocate_obj_tile_idx(4);
	update_life_display(PLAYER_LIFE_START);

	visual_ent_t *ent = allocate_visual_ent();

	ent->type = TYPE_VISUAL_LIFE;

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
}

void load_speed_level_display()
{
	_speed_level_tile_idx = allocate_obj_tile_idx(4);
	update_speed_level_display(PLAYER_AIR_START_SLOWDOWN);

	visual_ent_t *ent = allocate_visual_ent();

	ent->type = TYPE_VISUAL_SPEED_LEVEL;

	ent->x = (GBA_WIDTH - 16 * 2) * FIX_SCALE;
	ent->y = 0;

	ent->att.attr0 = ATTR0_SQUARE | ATTR0_8BPP;
	ent->att.attr1 = ATTR1_SIZE_16x16;
	ent->att.attr2 = ATTR2_PALBANK(0) | ATTR2_PRIO(0) | ATTR2_ID(_speed_level_tile_idx);
}

void update_speed_level_display(FIXED speed)
{
	FIXED inc = fxdiv(PLAYER_AIR_START_SLOWDOWN - PLAYER_AIR_SLOWDOWN_MIN, int2fx(_speed_level_tiles_length));

	for (int i = 0; i < _speed_level_tiles_length; i++)
	{
		if (speed <= PLAYER_AIR_SLOWDOWN_MIN + inc * i)
		{
			dma3_cpy(&tile_mem[4][_speed_level_tile_idx], _speed_level_tiles[i], speedLevel0TilesLen);
			return;
		}
	}

	dma3_cpy(&tile_mem[4][_speed_level_tile_idx], _speed_level_tiles[_speed_level_tiles_length - 1], speedLevel0TilesLen);
}

void init_jump_level_display()
{

	visual_ent_t *ent = allocate_visual_ent();

	ent->type = TYPE_VISUAL_SPEED_LEVEL;

	_jump_level_tile_idx = allocate_obj_tile_idx(4);

	ent->x = (GBA_WIDTH - 16 * 3) * FIX_SCALE;
	ent->y = 0;

	ent->att.attr0 = ATTR0_SQUARE | ATTR0_8BPP;
	ent->att.attr1 = ATTR1_SIZE_16x16;
	ent->att.attr2 = ATTR2_PALBANK(0) | ATTR2_PRIO(0) | ATTR2_ID(_jump_level_tile_idx);

	update_jump_level_display(PLAYER_START_JUMP_POWER);
}

void update_jump_level_display(FIXED jump_power)
{
	FIXED inc = fxdiv(PLAYER_MAX_JUMP_POWER - PLAYER_START_JUMP_POWER, 4 * FIX_SCALE);

	char str[50];
	for (int i = 0; i < 4; i++)
	{
		sprintf(str, "i:%d,p:%.2f,l:%.2f", i, fx2float(jump_power), fx2float(PLAYER_START_JUMP_POWER + inc * i));
		write_to_log(LOG_LEVEL_DEBUG, str);
		if (jump_power <= PLAYER_START_JUMP_POWER + inc * i)
		{
			dma3_cpy(&tile_mem[4][_jump_level_tile_idx], jumpLevel0Tiles + i * 64, jumpLevel0TilesLen / 4);
			return;
		}
	}
}

void free_jump_level_display()
{
	free_obj_tile_idx(_jump_level_tile_idx, 4);
}

void init_score_display()
{
	for (int i = 0; i < SCORE_DIGITS; i++)
	{
		_score_digs[i] = allocate_visual_ent();

		_score_digs[i]->type = TYPE_VISUAL_SCORE;

		_score_digs[i]->x = int2fx(8 * i);
		_score_digs[i]->y = 0;

		_score_digs[i]->att.attr0 = ATTR0_SQUARE | ATTR0_8BPP;
		_score_digs[i]->att.attr1 = ATTR1_SIZE_8x8;
		_score_digs[i]->att.attr2 = ATTR2_PALBANK(0) | ATTR2_PRIO(0) | ATTR2_ID(get_number_tile_start());
	}
}

void update_score_display(int score)
{
	//Count number of digits
	int w_score = score;
	int digit_count = 0;
	while (w_score != 0)
	{
		digit_count++;
		w_score /= 10;
	}

	w_score = score;
	for (int i = SCORE_DIGITS - 1; i >= 0; i--)
	{
		int offset;
		if (((SCORE_DIGITS - 1) - i) < digit_count)
		{
			offset = (w_score % 10);
		}
		else
		{
			offset = 0;
		}

		w_score /= 10;

		_score_digs[i]->att.attr2 =
			ATTR2_PALBANK(0) |
			ATTR2_PRIO(0) |
			ATTR2_ID(get_number_tile_start() + offset * 2);
	}
}

void free_score_display()
{
	for (int i = 0; i < SCORE_DIGITS; i++)
	{
		free_visual_ent(_score_digs[i]);
	}
}
