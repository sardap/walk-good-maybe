#include "special_zone.h"

#include <stdio.h>

#include <tonc.h>
#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

#include "../assets/szSharedBackground.h"
#include "../assets/szGrid00.h"
#include "../assets/szGrid01.h"

static const FIXED sz_max_scroll_speed = 0.75f * FIX_SCALEF;
static const int grid_background_tile = 1;
static const int grid_background_tile_offset = grid_background_tile * 64;

static sz_data_t _tmp;
static sz_data_t *_data = &_tmp;

static void show(void)
{
	_data->bg0_x = 0 / FIX_SCALE;
	_data->bg0_y = 0 / FIX_SCALE;
	_data->count = 300;
	_data->toggle = false;

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

	/* Load palettes */
	GRIT_CPY(pal_bg_mem, szSharedBackgroundPal);
	/* Load background tiles into SZ_SHARED_CB */
	memcpy16(
		&tile_mem[SZ_SHARED_CB] + grid_background_tile_offset,
		szGrid01Tiles, szGrid00TilesLen);
	//Map
	memset16(&se_mem[SZ_GRADIENT_SBB], grid_background_tile, SB_SIZE);

	REG_BG0CNT = BG_PRIO(0) | BG_8BPP | BG_SBB(SZ_GRADIENT_SBB) | BG_CBB(SZ_SHARED_CB) | BG_REG_32x32;

	// enable hblank register and set the mode7 type
	irq_init(NULL);
	irq_add(II_VBLANK, mmVBlank);

	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;
}

static void update(void)
{
	_data->count++;

	FIXED max = MAX(_data->bg0_dir_x, _data->bg0_dir_y) - 0.25f * FIX_SCALEF;

	if (_data->count > 10 + fx2int(fxmul(30 * FIX_SCALE, 1 * FIX_SCALE - (fxdiv(max, sz_max_scroll_speed)))))
	{
		if (_data->toggle)
			memcpy16(
				&tile_mem[SZ_SHARED_CB] + grid_background_tile_offset,
				szGrid00Tiles, szGrid00TilesLen);
		else
			memcpy16(
				&tile_mem[SZ_SHARED_CB] + grid_background_tile_offset,
				szGrid01Tiles, szGrid01TilesLen);

		_data->toggle = !_data->toggle;

		_data->count = 0;
	}

	_data->bg0_x += _data->bg0_dir_x;
	_data->bg0_y += _data->bg0_dir_y;

	REG_BG0HOFS = fx2int(_data->bg0_x);
	REG_BG0VOFS = fx2int(_data->bg0_y);
}

static void hide(void)
{
	REG_DISPCNT = 0;
}

const scene_t special_zone_scene = {
	.show = show,
	.update = update,
	.hide = hide};
