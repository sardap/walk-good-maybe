#include "game_over.h"

#include <tonc.h>
#include <string.h>
#include <maxmod.h>

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

	_data->score = _in_data.score;
	_data->timer = 0;
	_data->state = GO_STATES_BLACK;
	_data->bld_y = 16;

	GRIT_CPY(&tile8_mem[GO_SHARED_CB], goSharedBackgroundTiles);
	GRIT_CPY(&se_mem[GO_MAIN_SBB], goMainBackgroundMap);
	GRIT_CPY(&se_mem[GO_TEXT_SBB], goTextMap);

	REG_BG0HOFS = 0;
	REG_BG0VOFS = 0;

	REG_BG1HOFS = 0;
	REG_BG1VOFS = 0;

	REG_BG2HOFS = 0;
	REG_BG2VOFS = 0;

	REG_BG0CNT = BG_PRIO(2) | BG_8BPP | BG_SBB(GO_MAIN_SBB) | BG_CBB(GO_SHARED_CB) | BG_REG_32x32;
	REG_BG1CNT = BG_PRIO(1) | BG_8BPP | BG_SBB(GO_TEXT_SBB) | BG_CBB(GO_SHARED_CB) | BG_REG_32x32;

	GRIT_CPY(pal_bg_mem, goSharedBackgroundPal);

	// Update blend weights
	REG_DISPCNT = DCNT_MODE1 | DCNT_BG0 | DCNT_BG1;

	REG_BLDCNT = BLD_BUILD(
		BLD_BG0 | BLD_BG1, // Top layers
		0,				   // Bottom layers
		3				   // Mode
	);

	REG_BLDY = BLDY_BUILD(_data->bld_y);
}

static void update(void)
{
	_data->timer++;

	switch (_data->state)
	{
	case GO_STATES_BLACK:

		if (_data->timer > 2 * 60)
		{
			_data->timer = 0;
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

		if (key_hit(KEY_A))
		{
			scene_set(title_screen);
		}
		break;
	}
}

static void hide(void)
{
	REG_DISPCNT = 0;
	REG_BLDCNT = 0;
	mmStop();
}

const scene_t game_over_scene = {
	.show = show,
	.update = update,
	.hide = hide,
};
