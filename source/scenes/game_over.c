#include "game_over.h"

#include <tonc.h>
#include <string.h>

#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

#include "scene_shared.h"
#include "title_screen.h"
#include "../debug.h"

#include "../assets/goMainBackground.h"
#include "../assets/goSharedBackground.h"
#include "../assets/goText.h"

static go_data_t *_data = &_shared_data.go;

EWRAM_DATA static go_transfer_in_t _in_data;

void set_go_in(go_transfer_in_t data)
{
	_in_data = data;
}

static void show(void)
{
	load_blank();

	irq_add(II_VBLANK, mmVBlank);
	mmInitDefault((mm_addr)soundbank_bin, 8);

	_data->score = _in_data.score;
	_data->timer = 0;
	_data->state = GO_STATES_BLACK;
	_data->bld_y = 16;
	_data->volume = (mm_word)1024;

	GRIT_CPY(&tile8_mem[GO_SHARED_CB], goSharedBackgroundTiles);
	GRIT_CPY(&se_mem[GO_MAIN_SBB], goMainBackgroundMap);
	GRIT_CPY(&se_mem[GO_TEXT_SBB], goTextMap);

	REG_BG0HOFS = 0;
	REG_BG0VOFS = 0;

	REG_BG1HOFS = 0;
	REG_BG1VOFS = 0;

	REG_BG2HOFS = 0;
	REG_BG2VOFS = 0;

	REG_BG0CNT = BG_PRIO(0) | BG_8BPP | BG_SBB(GO_TEXT_SBB) | BG_CBB(GO_SHARED_CB) | BG_REG_32x32;
	REG_BG1CNT = BG_PRIO(0) | BG_8BPP | BG_SBB(GO_MAIN_SBB) | BG_CBB(GO_SHARED_CB) | BG_REG_32x32;

	// init map text
	txt_init_std();
	txt_init_obj(oam_mem, 0xF200, CLR_WHITE, 0x0E);
	// px between letters
	gptxt->dx = 10;
	gptxt->dy = 10;

	char str[50];
	sprintf(str, "SCORE: %d", _data->score);
	obj_puts2(
		75, 120, str,
		0xF200, _data->text_objs);
	oam_copy(oam_mem, _data->text_objs, CR_OBJ_COUNT);

	GRIT_CPY(pal_bg_mem, goSharedBackgroundPal);

	// Update blend weights
	REG_DISPCNT = DCNT_MODE1 | DCNT_BG0 | DCNT_BG1 | DCNT_OBJ;

	REG_BLDCNT = BLD_BUILD(
		BLD_BG0 | BLD_BG1 | BLD_OBJ, // Top layers
		0,							 // Bottom layers
		3							 // Mode
	);

	REG_BLDY = BLDY_BUILD(_data->bld_y);

	mmStart(MOD_PD_END, MM_PLAY_ONCE);
}

static void update(void)
{
	_data->timer++;

	switch (_data->state)
	{
	case GO_STATES_BLACK:
		if (_data->timer > 2 * 60)
		{
			_data->state = GO_STATES_FADING_IN;
		}
		break;
	case GO_STATES_FADING_IN:
		if (frame_count() % 8 == 0)
		{
			_data->bld_y = CLAMP(_data->bld_y - 1, 0, 17);
			REG_BLDY = BLDY_BUILD(_data->bld_y);
		}

		if (_data->bld_y == 0)
		{
			_data->state = GO_STATES_SOLID;
		}
		break;
	case GO_STATES_SOLID:
		if (key_hit(KEY_A) && _data->timer > 300)
		{
			scene_set(_title_scene);
		}
		break;
	}

	int vol_dec = 0;

	if (_data->timer > 270)
	{
		vol_dec = 5;
	}
	else if (_data->state == GO_STATES_FADING_IN)
	{
		vol_dec = 1;
	}

	_data->volume = CLAMP(_data->volume - vol_dec, 0, 1024);
	mmSetModuleVolume((mm_word)_data->volume);
#ifdef DEBUG
	char str[50];
	sprintf(str, "%d %d", _data->volume, _data->timer);
	write_to_log(LOG_LEVEL_DEBUG, str);
#endif
}

static void hide(void)
{
	load_blank();
	REG_BLDCNT = 0;
	OAM_CLEAR();
	mmSetModuleVolume((mm_word)1024);
}

const scene_t _game_over_scene = {
	.show = show,
	.update = update,
	.hide = hide,
};
