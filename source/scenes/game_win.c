#include "game_win.h"

#include <tonc.h>
#include <string.h>
#include <limits.h>

#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

#include "scene_shared.h"
#include "credits.h"
#include "../sound.h"
#include "../debug.h"

#include "../assets/gwSharedBackground.h"
#include "../assets/gwBandit.h"
#include "../assets/gwVictory.h"

static gw_data_t *_data = &_shared_data.gw;

static void show(void)
{
	load_blank();

	irq_add(II_VBLANK, mmVBlank);

	_data->timer = 0;
	_data->state = GW_STATES_WHITE;
	_data->bld_y = 17;
	_data->vict_x = 0;

	GRIT_CPY(&tile8_mem[GW_SHARED_CB], gwSharedBackgroundTiles);
	GRIT_CPY(&se_mem[GW_MAIN_SBB], gwBanditMap);
	GRIT_CPY(&se_mem[GW_TEXT_SBB], gwVictoryMap);

	REG_BG0HOFS = 0;
	REG_BG0VOFS = 0;

	REG_BG1HOFS = int2fx(_data->vict_x);
	REG_BG1VOFS = 0;

	REG_BG0CNT = BG_PRIO(1) | BG_8BPP | BG_SBB(GW_MAIN_SBB) | BG_CBB(GW_SHARED_CB) | BG_REG_32x32;
	REG_BG1CNT = BG_PRIO(0) | BG_8BPP | BG_SBB(GW_TEXT_SBB) | BG_CBB(GW_SHARED_CB) | BG_REG_64x32;

	// init map text
	txt_init_std();
	txt_init_obj(oam_mem, 0xF200, CLR_WHITE, 0x0E);
	// px between letters
	gptxt->dx = 10;
	gptxt->dy = 10;

	GRIT_CPY(pal_bg_mem, gwSharedBackgroundPal);
	REG_DISPCNT = DCNT_MODE1 | DCNT_BG0 | DCNT_BG1;

	REG_BLDCNT = BLD_BUILD(
		BLD_BG0 | DCNT_BG1, // Top layers
		0,					// Bottom layers
		2					// Mode
	);

	REG_BLDY = BLDY_BUILD(_data->bld_y);

	mmStart(MOD_PD_VICTORY, MM_PLAY_ONCE);
}

static void update(void)
{
	_data->timer++;

	switch (_data->state)
	{
	case GW_STATES_WHITE:
		if (_data->timer > 30)
		{
			_data->state = GO_STATES_FADING_IN;
		}
		break;
	case GW_STATES_FADING_IN:
		if (frame_count() % 4 == 0)
		{
			_data->bld_y = CLAMP(_data->bld_y - 1, 0, 17);
			REG_BLDY = BLDY_BUILD(_data->bld_y);
		}

		if (_data->bld_y <= 0)
		{
			_data->state = GW_STATES_SLIDING;
		}
		break;
	case GW_STATES_SLIDING:
#ifdef DEBUG
		char str[50];
		sprintf(str, "FUCK:%d", fx2int(_data->vict_x));
		write_to_log(LOG_LEVEL_DEBUG, str);
#endif
		_data->vict_x = CLAMP(_data->vict_x + 1.0f * FIX_SCALE, 0, INT_MAX);
		REG_BG1HOFS = fx2int(_data->vict_x);

		if (_data->vict_x >= 260 * FIX_SCALE)
		{
			_data->state = GW_STATES_COMPLETE;
		}
		break;
	case GW_STATES_COMPLETE:
		if (key_hit(KEY_A) && !mmActive())
		{
			scene_set(_credits_scene);
		}
		break;
	}
}

static void hide(void)
{
	load_blank();
	REG_BLDCNT = 0;
	OAM_CLEAR();
	setModuleVolume(MUSIC_MAX_VOLUME);
}

const scene_t _game_win_scene = {
	.show = show,
	.update = update,
	.hide = hide,
};
