#include "title_screen.h"

#include <stdlib.h>

#include "game_intro.h"
#include "credits.h"
#include "../debug.h"
#include "../graphics.h"

#include "../assets/titleScreenShared.h"
#include "../assets/tsCity.h"
#include "../assets/tsBeach.h"
#include "../assets/tsTitleText.h"
#include "../assets/tsLava.h"
#include "../assets/tsGameText.h"
#include "../assets/tsCredits.h"
#include "../assets/tsArrow.h"
#include "../assets/tsArrowRed.h"
#include "../assets/tsSpirteShared.h"

const ts_menu_options_t _options[] = {
	{&game_intro, tsGameTextMap, tsGameTextMapLen, 51, 160},
	{&credits_screen, tsCreditsMap, tsCreditsMapLen, 23, 190},
};

static const u16 water_cycle[] = {0x7FFF, 0x7FC6, 0x7B80, 0x7FD0, 0x7FB2, 0x7FD7};
static const u16 lava_cycle[] = {0x11D9, 0x1E3C, 0x20FF};

static FIXED _bkg_x;
static int _water_pal_idx;
static int _lava_pal_idx;
static int _active_opt_idx;
static OBJ_ATTR _arrow_objs[2];
static OBJ_ATTR *_left_arrow = &_arrow_objs[0];
static OBJ_ATTR *_right_arrow = &_arrow_objs[1];
static int _pal_change_countdown;

static void show(void)
{
	// Load palette
	GRIT_CPY(pal_bg_mem, titleScreenSharedPal);
	GRIT_CPY(pal_obj_mem, tsSpirteSharedPal);

	// Load tiles into shared_cb
	LZ77UnCompVram(titleScreenSharedTiles, &tile_mem[TS_SHARED_CB]);
	//sprite tiles
	GRIT_CPY(&tile_mem[4], tsArrowTiles);
	GRIT_CPY(&tile_mem[4][tsArrowTilesLen / 32], tsArrowRedTiles);

	// Load maps
	GRIT_CPY(&se_mem[TS_CITY_SSB], tsCityMap);
	GRIT_CPY(&se_mem[TS_BEACH_SSB], tsBeachMap);
	GRIT_CPY(&se_mem[TS_TEXT_SSB], tsTitleTextMap);

	//Set offsets
	REG_BG0HOFS = 0;
	REG_BG0VOFS = 0;

	REG_BG1HOFS = 0;
	REG_BG1VOFS = 0;

	REG_BG2HOFS = 0;
	REG_BG2VOFS = 0;

	REG_BG3HOFS = 0;
	REG_BG3VOFS = 0;

	//Set bkg reg
	REG_BG0CNT = BG_PRIO(1) | BG_8BPP | BG_SBB(TS_CITY_SSB) | BG_CBB(TS_SHARED_CB) | BG_REG_32x32;
	REG_BG1CNT = BG_PRIO(1) | BG_8BPP | BG_SBB(TS_BEACH_SSB) | BG_CBB(TS_SHARED_CB) | BG_REG_32x32;
	REG_BG2CNT = BG_PRIO(0) | BG_8BPP | BG_SBB(TS_TEXT_SSB) | BG_CBB(TS_SHARED_CB) | BG_REG_32x32;
	REG_BG3CNT = BG_PRIO(0) | BG_8BPP | BG_SBB(TS_OPTION_SSB) | BG_CBB(TS_SHARED_CB) | BG_REG_32x32;

	//Window
	REG_WIN0H = 0 << 8 | 120;
	REG_WIN0V = 0 << 8 | 160;

	REG_WININ = WIN_BG0 | WIN_BG2 | WIN_BG3 | WIN_OBJ;
	REG_WINOUT = WIN_BUILD(WIN_BG1 | WIN_BG2 | WIN_BG3 | WIN_OBJ, 0);

	//Display Reg
	REG_DISPCNT = DCNT_OBJ_1D | DCNT_OBJ | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_BG3 | DCNT_WIN0 | DCNT_WIN1;

	_bkg_x = gba_rand();
	_active_opt_idx = 0;
	_water_pal_idx = 0;
	_lava_pal_idx = 0;

	//Copy active menu item into OPTION SBB
	memcpy16(
		&se_mem[TS_OPTION_SSB],
		_options[_active_opt_idx].map,
		_options[_active_opt_idx].mapLen);

	//Left arrow
	obj_set_attr(
		_left_arrow,
		ATTR0_SQUARE | ATTR0_8BPP,
		ATTR1_SIZE_32x32 | ATTR1_HFLIP,
		ATTR2_ID(0) | ATTR2_PRIO(0));
	obj_set_pos(_left_arrow, _options[_active_opt_idx].lx, TS_ARROW_Y);

	//Left arrow
	obj_set_attr(
		_right_arrow,
		ATTR0_SQUARE | ATTR0_8BPP,
		ATTR1_SIZE_32x32,
		ATTR2_ID(0) | ATTR2_PRIO(0));
	obj_set_pos(_right_arrow, _options[_active_opt_idx].rx, TS_ARROW_Y);
}

static void update(void)
{
	_bkg_x += 0.3f * FIX_SCALE;

	REG_BG0HOFS = fx2int(_bkg_x);
	REG_BG1HOFS = fx2int(-_bkg_x);

	if (key_hit(KEY_LEFT) || key_hit(KEY_RIGHT))
	{
		_active_opt_idx = wrap(_active_opt_idx + key_tri_horz(), 0, TS_OPTIONS_LENGTH);
		//Copy active menu item into OPTION SBB
		memcpy16(
			&se_mem[TS_OPTION_SSB],
			_options[_active_opt_idx].map,
			_options[_active_opt_idx].mapLen);

		obj_set_pos(_left_arrow, _options[_active_opt_idx].lx, TS_ARROW_Y);
		obj_set_pos(_right_arrow, _options[_active_opt_idx].rx, TS_ARROW_Y);

		//Now you can't do pallte stuff here since they both use the same colour in the pallte
		if (key_hit(KEY_LEFT))
			_left_arrow->attr2 = ATTR2_ID(tsArrowTilesLen / 32);
		else
			_right_arrow->attr2 = ATTR2_ID(tsArrowTilesLen / 32);

		_pal_change_countdown = 5;
	}

	_pal_change_countdown = clamp(_pal_change_countdown - 1, 0, 100);
	//change back to ogrinal arrow
	if (_pal_change_countdown <= 0)
	{
		_left_arrow->attr2 = ATTR2_ID(0);
		_right_arrow->attr2 = ATTR2_ID(0);
	}

	if (key_hit(KEY_A))
	{
		for (int i = 0; i < frame_count(); i++)
		{
			qran();
		}

		scene_set(*_options[_active_opt_idx].scene);
	}

	if (frame_count() % 25 == 0)
	{
		cycle_palate(
			pal_bg_mem,
			TS_PAL_WATER_START, water_cycle,
			&_water_pal_idx, TS_PAL_WATER_LENGTH);
	}

	if (frame_count() % 35 == 0)
	{
		cycle_palate(
			pal_bg_mem,
			TS_PAL_LAVA_START, lava_cycle,
			&_lava_pal_idx, TS_PAL_LAVA_LENGTH);
	}

	obj_copy(oam_mem, _arrow_objs, 2);
}

static void hide(void)
{
	REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D | DCNT_BG0;
	OAM_CLEAR();
}

const scene_t title_screen = {
	.show = show,
	.update = update,
	.hide = hide};
