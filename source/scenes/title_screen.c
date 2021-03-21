#include "title_screen.h"

#include "game_intro.h"
#include "../debug.h"

#include "../assets/titleScreenShared.h"
#include "../assets/tsCity.h"
#include "../assets/tsBeach.h"
#include "../assets/tsTitleText.h"

static const u16 water_cycle[] = {0x7FFF, 0x7FC6, 0x7B80, 0x7FD0, 0x7FB2, 0x7FD7};

static FIXED _bkg_x;
static int _water_pal_idx;

static void show(void)
{
	// Load palette
	GRIT_CPY(pal_bg_mem, titleScreenSharedPal);

	for (int i = 0; i < TS_PAL_WATER_LENGTH; i++)
	{
		pal_bg_mem[TS_PAL_WATER_START + i] = water_cycle[i];
	}

	// Load tiles into shared_cb
	GRIT_CPY(&tile_mem[TS_SHARED_CB], titleScreenSharedTiles);
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

	//Set bkg reg
	REG_BG0CNT = BG_PRIO(1) | BG_8BPP | BG_SBB(TS_CITY_SSB) | BG_CBB(TS_SHARED_CB) | BG_REG_32x32;
	REG_BG1CNT = BG_PRIO(1) | BG_8BPP | BG_SBB(TS_BEACH_SSB) | BG_CBB(TS_SHARED_CB) | BG_REG_32x32;
	REG_BG2CNT = BG_PRIO(0) | BG_8BPP | BG_SBB(TS_TEXT_SSB) | BG_CBB(TS_SHARED_CB) | BG_REG_32x32;

	//Window
	REG_WIN0H = 0 << 8 | 120;
	REG_WIN0V = 0 << 8 | 160;

	REG_WININ = WIN_BG0 | WIN_BG2;
	REG_WINOUT = WIN_BUILD(WIN_BG1 | WIN_BG2, 0);

	//Display Reg
	REG_DISPCNT = DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_WIN0 | DCNT_WIN1;
}

static void update(void)
{
	_bkg_x += 0.3f * FIX_SCALE;

	REG_BG0HOFS = fx2int(_bkg_x);
	REG_BG1HOFS = fx2int(-_bkg_x);

	if (key_hit(KEY_A) || key_hit(KEY_START))
	{
		for (int i = 0; i < frame_count(); i++)
		{
			qran();
		}
		scene_set(game_intro);
	}

	if (frame_count() % 25 == 0)
	{
		for (int i = 0; i < TS_PAL_WATER_LENGTH; i++)
		{
			int pal_idx = wrap(_water_pal_idx + i, 0, TS_PAL_WATER_LENGTH - 1);

			pal_bg_mem[TS_PAL_WATER_START + i] = water_cycle[pal_idx];
		}
		_water_pal_idx = wrap(_water_pal_idx + 1, 0, TS_PAL_WATER_LENGTH - 1);
	}
}

static void hide(void)
{
	REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D | DCNT_BG0;
}

const scene_t title_screen = {
	.show = show,
	.update = update,
	.hide = hide};
