#include "special_zone.h"

#include <stdio.h>

#include <tonc.h>
#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

#include "../debug.h"

#include "../assets/szSharedBackground.h"
#include "../assets/szGrid00.h"
#include "../assets/szGrid01.h"
#include "../assets/szEye00.h"
#include "../assets/szEye01.h"
#include "../assets/szText.h"

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
static const int text_tile = 40;

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

static void update_text_fade(FIXED max)
{
	int base;
	switch (_data->text_state)
	{
	case FADED:
		base = 120;
		break;
	case UNFDAING:
	case FADING:
		base = 5;
	default:
		break;
	}

	_data->text_fade_count++;

	if (_data->text_fade_count < base + fx2int(fxmul(base * FIX_SCALE, 1 * FIX_SCALE - (fxdiv(max, sz_max_scroll_speed)))))
		return;

	_data->text_fade_count = 0;

	switch (_data->text_state)
	{
	case FADED:
		_data->text_state = UNFDAING;
		break;
	case UNFDAING:
		_data->text_eva++;
		if (_data->text_eva >= 15)
			_data->text_state = FADING;
		break;
	case FADING:
		_data->text_eva--;
		if (_data->text_eva < 0)
		{
			_data->text_eva = 0;
			_data->text_state = FADED;
		}
		break;
	}

	// Transparent
	REG_BLDCNT = BLD_BUILD(
		BLD_BG2,					 // Top layers
		BLD_BG0 | BLD_BG1 | BLD_OBJ, // Bottom layers
		1							 // Mode (std)
	);

	/* Update blend weights
		Left EVA: Top weight max of 15 (4 bits)
		Right EVB: Bottom wieght max of 15 (4 bits)
		almost complete fade
	*/
	REG_BLDALPHA = BLDA_BUILD(_data->text_eva, 15 - _data->text_eva);
	REG_BLDY = BLDY_BUILD(0);
}

static void show(void)
{
	_data->bg0_x = 0 / FIX_SCALE;
	_data->bg0_y = 0 / FIX_SCALE;
	_data->grid_count = 300;
	_data->eye_count = 300;
	_data->text_fade_count = 300;
	_data->text_eva = 0;
	_data->text_state = FADED;
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
	REG_BG0HOFS = fx2int(_data->bg0_x);
	REG_BG0VOFS = fx2int(_data->bg0_y);

	REG_BG1HOFS = fx2int(_data->bg0_x);
	REG_BG1VOFS = fx2int(_data->bg0_y);

	REG_BG3HOFS = fx2int(_data->bg0_x);
	REG_BG3VOFS = fx2int(_data->bg0_y);

	/* Load palettes */
	GRIT_CPY(pal_bg_mem, szSharedBackgroundPal);
	/* Load background tiles into SZ_SHARED_CB */
#ifdef DEBUG
	memset16(
		&tile_mem[SZ_SHARED_CB],
		0, 800 * 64);
#endif
	memcpy16(
		&tile8_mem[SZ_SHARED_CB],
		szGrid01Tiles, szGrid00TilesLen / 2);

	memcpy16(
		&tile8_mem[SZ_SHARED_CB][eye_tile],
		szEye00Tiles, szEye00TilesLen / 2);

	memcpy16(
		&tile8_mem[SZ_SHARED_CB][text_tile],
		szTextTiles, szTextTilesLen / 2);

	// Map
	memset16(&se_mem[SZ_GRID_SBB], grid_background_tile, SB_SIZE);
	memset16(&se_mem[SZ_EYE_SBB], 0, SB_SIZE);

	// Spawn eyes
	for (int i = 0; i < gba_rand_range(4, 6); i++)
	{
		int x_ofs = gba_rand_range(0, 30 - eye_map_col_len);
		int y_ofs = gba_rand_range(0, 30 - eye_map_row_len);
		make_eye(x_ofs, y_ofs);
	}

	for (int i = 0; i < szTextMapLen / 2; i++)
	{
		unsigned int tile = szTextMap[i] ? szTextMap[i] + text_tile : 0;
		se_mem[SZ_TEXT_SBB][i] = tile;
	}

	// BG regs
	REG_BG0CNT = BG_PRIO(SZ_GRID_LAYER) | BG_8BPP | BG_SBB(SZ_GRID_SBB) | BG_CBB(SZ_SHARED_CB) | BG_REG_32x32;
	REG_BG1CNT = BG_PRIO(SZ_EYE_LAYER) | BG_8BPP | BG_SBB(SZ_EYE_SBB) | BG_CBB(SZ_SHARED_CB) | BG_REG_32x32;
	REG_BG2CNT = BG_PRIO(SZ_TEXT_LAYER) | BG_8BPP | BG_SBB(SZ_TEXT_SBB) | BG_CBB(SZ_SHARED_CB) | BG_REG_32x32;

	// Transparent
	REG_BLDCNT = BLD_BUILD(
		BLD_BG2,					 // Top layers
		BLD_BG0 | BLD_BG1 | BLD_OBJ, // Bottom layers
		1							 // Mode (std)
	);

	/* Update blend weights
		Left EVA: Top weight max of 15 (4 bits)
		Right EVB: Bottom wieght max of 15 (4 bits)
		almost complete fade
	*/
	REG_BLDALPHA = BLDA_BUILD(0, 15);
	REG_BLDY = BLDY_BUILD(0);

	update_text_fade(1);

	// enable hblank register and set the mode7 type
	irq_init(NULL);
	irq_add(II_VBLANK, mmVBlank);

	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_BG3;
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

	_data->bg0_x += _data->bg0_dir_x;
	_data->bg0_y += _data->bg0_dir_y;

	REG_BG0HOFS = fx2int(_data->bg0_x);
	REG_BG0VOFS = fx2int(_data->bg0_y);

	// Blinking
	_data->eye_count++;

	if (_data->eye_count > 10 + fx2int(fxmul(60 * FIX_SCALE, 1 * FIX_SCALE - (fxdiv(max, sz_max_scroll_speed)))))
	{
		blink_eye();
		_data->eye_count = 0;
	}

	REG_BG1HOFS = -fx2int(fxmul(_data->bg0_x, 0.5f * FIX_SCALEF));
	REG_BG1VOFS = -fx2int(fxmul(_data->bg0_y, 0.5f * FIX_SCALEF));

	// Text
	update_text_fade(max);
}

static void hide(void)
{
	REG_DISPCNT = 0;

	REG_BLDCNT = BLD_BUILD(
		BLD_OFF, // Top layers
		BLD_OFF, // Bottom layers
		BLD_OFF	 // Mode (off)
	);
}

const scene_t special_zone_scene = {
	.show = show,
	.update = update,
	.hide = hide};
