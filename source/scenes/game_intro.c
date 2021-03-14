#include "game_intro.h"

#include <tonc.h>
#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

#include "main_game.h"
#include "../graphics.h"

#include "../assets/ready.h"
#include "../assets/set.h"
#include "../assets/go.h"
#include "../assets/whaleFar.h"
#include "../assets/whaleClose.h"
#include "../assets/whaleVeryClose.h"
#include "../assets/gameIntroShared.h"
#include "../assets/giCoolBackground.h"

static const int shared_cb = 0;
static const int text_ssb = 30;
static const int whale_ssb = 28;
static const int cool_background_ssb = 26;
static const mm_sfxhand _intro_handler = 1;

static gi_states_t _state;
static POINT _pt_bg;

static void show(void)
{
	// Load palette
	dma3_cpy(pal_bg_mem, gameIntroSharedPal, gameIntroSharedPalLen);
	// Load tiles into shared_cb
	dma3_cpy(&tile_mem[shared_cb], gameIntroSharedTiles, gameIntroSharedTilesLen);
	// Copy title text to menu
	dma3_cpy(&se_mem[text_ssb], readyMap, readyMapLen);
	//Copy whale
	dma3_cpy(&se_mem[whale_ssb], whaleFarMap, whaleFarMapLen);
	//Copy cool background
	dma3_cpy(&se_mem[cool_background_ssb], giCoolBackgroundMap, giCoolBackgroundMapLen);

	// Set RegX scroll to 0
	REG_BG0HOFS = 0;
	REG_BG0VOFS = 0;
	REG_BG1HOFS = 0;
	REG_BG1VOFS = 0;
	REG_BG1HOFS = 0;
	REG_BG2VOFS = gba_rand_range(0, 256);
	REG_BG2HOFS = 0;

	// Set bkg reg
	REG_BG0CNT = BG_PRIO(0) | BG_8BPP | BG_SBB(text_ssb) | BG_CBB(shared_cb) | BG_REG_32x32;
	REG_BG1CNT = BG_PRIO(1) | BG_8BPP | BG_SBB(whale_ssb) | BG_CBB(shared_cb) | BG_REG_32x32;
	REG_BG2CNT = BG_PRIO(2) | BG_8BPP | BG_SBB(cool_background_ssb) | BG_CBB(shared_cb) | BG_REG_32x32 | BG_MOSAIC;

	REG_DISPCNT = DCNT_OBJ_1D | DCNT_BG0 | DCNT_BG1 | DCNT_BG2;

	_state = GI_S_READY;
	_pt_bg.x = 0;
	_pt_bg.y = 0;

	mm_sound_effect shoot_sound = {
		{SFX_READY_0},
		(int)(1.0f * (1 << 10)),
		_intro_handler,
		120,
		127,
	};
	mmEffectEx(&shoot_sound);
}

static void update(void)
{
	// _pt_bg.x += 3;
	_pt_bg.y += gba_rand_range(3, 10);

	REG_MOSAIC = MOS_BUILD(_pt_bg.x >> 3, _pt_bg.y >> 3, 0, 0);

	if (mmEffectActive(_intro_handler))
		return;

	switch (_state)
	{
	case GI_S_READY:
		dma3_cpy(&se_mem[text_ssb], setMap, setMapLen);
		dma3_cpy(&se_mem[whale_ssb], whaleCloseMap, whaleCloseMapLen);

		mm_sound_effect set_sound = {
			{SFX_SET_0},
			(int)(1.0f * (1 << 10)),
			_intro_handler,
			120,
			127,
		};
		mmEffectEx(&set_sound);
		_state = GI_S_SET;

		break;
	case GI_S_SET:
		dma3_cpy(&se_mem[text_ssb], goMap, goMapLen);
		dma3_cpy(&se_mem[whale_ssb], whaleVeryCloseMap, whaleVeryCloseMapLen);

		mm_sound_effect go_sound = {
			{SFX_GO_0},
			(int)(1.0f * (1 << 10)),
			_intro_handler,
			120,
			127,
		};
		mmEffectEx(&go_sound);
		mmSetModuleVolume(300);
		mmStart(MOD_INTRO, MM_PLAY_ONCE);
		_state = GI_S_GO;
		scene_set(main_game);
		break;
	}
}

static void hide(void)
{
	REG_DISPCNT = 0;
}

const scene_t game_intro = {
	.show = show,
	.update = update,
	.hide = hide};
