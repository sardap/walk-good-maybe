#include "special_zone.h"

#include <stdio.h>

#include <tonc.h>
#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

#include "../assets/szSharedBackground.h"
#include "../assets/szGrid00.h"
#include "../assets/szGrid01.h"
#include "../assets/szEye00.h"
#include "../assets/szEye01.h"

static const int eye_map_row_len = 4;
static const int eye_map_col_len = 8;
static const unsigned int *eye_map[] = {
	(unsigned int[]){0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0000},
	(unsigned int[]){0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F},
	(unsigned int[]){0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017},
	(unsigned int[]){0x0000, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E}};

static const FIXED sz_max_scroll_speed = 0.75f * FIX_SCALEF;
static const int grid_background_tile = 1;
static const int eye_tile = 7;

static sz_data_t _tmp;
static sz_data_t *_data = &_tmp;

static void blink_eye()
{
	if (_data->eye_toggle)
		memcpy16(
			&tile8_mem[SZ_SHARED_CB][eye_tile],
			szEye00Tiles, szEye00TilesLen / 2);
	else
		memcpy16(
			&tile8_mem[SZ_SHARED_CB][eye_tile],
			szEye01Tiles, szEye01TilesLen / 2);

	_data->eye_toggle = !_data->eye_toggle;
}

static void make_eye(int x_ofs, int y_ofs)
{
	// Check to see if it's empty space
	for (int y = 0; y < eye_map_row_len; y++)
	{
		for (int x = 0; x < eye_map_col_len; x++)
		{
			if (se_mem[SZ_EYE_SBB][32 * (y + y_ofs) + (x + x_ofs)])
				return;
		}
	}

	for (int y = 0; y < eye_map_row_len; y++)
	{
		for (int x = 0; x < eye_map_col_len; x++)
		{
			unsigned int tile = eye_map[y][x] ? eye_map[y][x] + eye_tile : 0;
			se_mem[SZ_EYE_SBB][32 * (y + y_ofs) + (x + x_ofs)] = tile;
		}
	}
}

static void show(void)
{
	_data->bg0_x = 0 / FIX_SCALE;
	_data->bg0_y = 0 / FIX_SCALE;
	_data->grid_count = 300;
	_data->eye_count = 300;
	_data->grid_toggle = false;
	_data->eye_toggle = false;

	// Set direction
	_data->bg0_dir_x = float2fx(RAND_FLOAT(0.75f)) + 0.25f * FIX_SCALEF;
	if (gba_rand() % 2 == 0)
		_data->bg0_dir_x = -_data->bg0_dir_x;

	_data->bg0_dir_y = float2fx(RAND_FLOAT(0.75f)) + 0.25f * FIX_SCALEF;
	if (gba_rand() % 2 == 0)
		_data->bg0_dir_x = -_data->bg0_dir_x;

	// Set RegX scroll to 0
	REG_BG0HOFS = _data->bg0_x;
	REG_BG0VOFS = _data->bg0_y;

	// Set RegX scroll to 0
	REG_BG1HOFS = 0;
	REG_BG1VOFS = 0;

	/* Load palettes */
	GRIT_CPY(pal_bg_mem, szSharedBackgroundPal);
	/* Load background tiles into SZ_SHARED_CB */
	memset16(
		&tile_mem[SZ_SHARED_CB],
		0, 800 * 64);

	memcpy16(
		&tile8_mem[SZ_SHARED_CB],
		szGrid01Tiles, szGrid00TilesLen / 2);
	memcpy16(
		&tile8_mem[SZ_SHARED_CB][eye_tile],
		szEye00Tiles, szEye00TilesLen / 2);
	//Map
	memset16(&se_mem[SZ_GRID_SBB], grid_background_tile, SB_SIZE);
	memset16(&se_mem[SZ_EYE_SBB], 0, SB_SIZE);

	for (int i = 0; i < gba_rand_range(4, 6); i++)
	{
		int x_ofs = gba_rand_range(0, 32 - eye_map_row_len);
		int y_ofs = gba_rand_range(0, 32 - eye_map_col_len);
		make_eye(x_ofs, y_ofs);
	}

	REG_BG0CNT = BG_PRIO(SZ_GRID_LAYER) | BG_8BPP | BG_SBB(SZ_GRID_SBB) | BG_CBB(SZ_SHARED_CB) | BG_REG_32x32;
	REG_BG1CNT = BG_PRIO(SZ_EYE_LAYER) | BG_8BPP | BG_SBB(SZ_EYE_SBB) | BG_CBB(SZ_SHARED_CB) | BG_REG_32x32;

	// enable hblank register and set the mode7 type
	irq_init(NULL);
	irq_add(II_VBLANK, mmVBlank);

	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1;
}

static void update(void)
{
	_data->grid_count++;

	// Grid
	FIXED max = MAX(_data->bg0_dir_x, _data->bg0_dir_y) - 0.25f * FIX_SCALEF;

	if (_data->grid_count > 10 + fx2int(fxmul(30 * FIX_SCALE, 1 * FIX_SCALE - (fxdiv(max, sz_max_scroll_speed)))))
	{
		if (_data->grid_toggle)
			memcpy16(
				&tile_mem[SZ_SHARED_CB],
				szGrid00Tiles, szGrid00TilesLen);
		else
			memcpy16(
				&tile_mem[SZ_SHARED_CB],
				szGrid01Tiles, szGrid01TilesLen);

		_data->grid_toggle = !_data->grid_toggle;

		_data->grid_count = 0;
	}

	_data->eye_count++;

	// Blinking
	if (_data->eye_count > 50)
	{
		blink_eye();

		_data->eye_count = 0;
	}

	// Grid
	_data->bg0_x += _data->bg0_dir_x;
	_data->bg0_y += _data->bg0_dir_y;

	REG_BG0HOFS = fx2int(_data->bg0_x);
	REG_BG0VOFS = fx2int(_data->bg0_y);

	// Eye
	REG_BG1HOFS = -fx2int(fxmul(_data->bg0_x, 0.5f * FIX_SCALEF));
	REG_BG1VOFS = -fx2int(fxmul(_data->bg0_y, 0.5f * FIX_SCALEF));
}

static void hide(void)
{
	REG_DISPCNT = 0;
}

const scene_t special_zone_scene = {
	.show = show,
	.update = update,
	.hide = hide};
