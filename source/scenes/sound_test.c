#include "sound_test.h"

#include <tonc.h>
#include <string.h>
#include <maxmod.h>

#include "soundbank.h"
#include "soundbank_bin.h"

#include "scene_shared.h"
#include "title_screen.h"

#include "../sound.h"
#include "../debug.h"

#include "../assets/stBackground.h"
#include "../assets/stSharedBackground.h"
#include "../assets/stSpirteShared.h"
#include "../assets/stArrow.h"
#include "../assets/stArrowRed.h"

st_data_t *_data = &_shared_data.st;

static void show(void)
{
	irq_init(NULL);
	irq_add(II_VBLANK, mmVBlank);

	// Load palette
	LZ77UnCompVram(stSharedBackgroundPal, pal_bg_mem);
	GRIT_CPY(pal_obj_mem, stSpirteSharedPal);

	// Load tiles into shared_cb
	//Load background tiles
	LZ77UnCompVram(stSharedBackgroundTiles, &tile_mem[ST_SHARED_CB]);
	//Load tiles
	GRIT_CPY(&tile_mem[4], stArrowTiles);
	GRIT_CPY(&tile_mem[4][stArrowTilesLen / 32], stArrowRedTiles);

	// Load maps
	LZ77UnCompVram(stBackgroundMap, &se_mem[ST_COLOUR_SBB]);

	//Set offsets
	REG_BG0HOFS = 0;
	REG_BG0VOFS = 0;

	//Set bkg reg
	REG_BG0CNT = BG_PRIO(0) | BG_8BPP | BG_SBB(ST_COLOUR_SBB) | BG_CBB(ST_SHARED_CB) | BG_REG_32x32;

	//Display Reg
	REG_DISPCNT = DCNT_OBJ_1D | DCNT_OBJ | DCNT_BG0;

	// init map text
	txt_init_std();
	txt_init_obj(oam_mem, 0xF200, CLR_WHITE, 0x0E);
	// 12 px between letters
	gptxt->dx = 10;
	gptxt->dy = 10;

	//Obj stuff
	obj_hide_multi(oam_mem, 128);
	obj_hide_multi(_data->objs, ST_OBJ_COUNT);

	_data->sound_fx_left_arrow = &_data->objs[0];
	_data->sound_fx_right_arrow = &_data->objs[1];

	_data->sound_mus_left_arrow = &_data->objs[2];
	_data->sound_mus_right_arrow = &_data->objs[3];

	_data->selection_arrow = &_data->objs[4];

	//Selction arrow
	obj_set_attr(
		_data->selection_arrow,
		ATTR0_SQUARE | ATTR0_8BPP,
		ATTR1_SIZE_16,
		ATTR2_ID(0) | ATTR2_PRIO(0));
	obj_set_pos(_data->selection_arrow, 5, 35);

	//Fx Left arrow
	obj_set_attr(
		_data->sound_fx_left_arrow,
		ATTR0_SQUARE | ATTR0_8BPP,
		ATTR1_SIZE_16 | ATTR1_HFLIP,
		ATTR2_ID(0) | ATTR2_PRIO(0));
	obj_set_pos(_data->sound_fx_left_arrow, 86, 35);

	//Fx Right arrow
	obj_set_attr(
		_data->sound_fx_right_arrow,
		ATTR0_SQUARE | ATTR0_8BPP,
		ATTR1_SIZE_16,
		ATTR2_ID(0) | ATTR2_PRIO(0));
	obj_set_pos(_data->sound_fx_right_arrow, 125, 35);

	//Music Left arrow
	obj_set_attr(
		_data->sound_mus_left_arrow,
		ATTR0_SQUARE | ATTR0_8BPP,
		ATTR1_SIZE_16 | ATTR1_HFLIP,
		ATTR2_ID(0) | ATTR2_PRIO(0));
	obj_set_pos(_data->sound_mus_left_arrow, 86, 80);

	//Music Right arrow
	obj_set_attr(
		_data->sound_mus_right_arrow,
		ATTR0_SQUARE | ATTR0_8BPP,
		ATTR1_SIZE_16,
		ATTR2_ID(0) | ATTR2_PRIO(0));
	obj_set_pos(_data->sound_mus_right_arrow, 125, 80);

	_data->arrow_countdown = 0;

	_data->fx_idx = 0;
	_data->fx_mode = ST_SELECTION_FX;

	_data->mus_idx = 0;
}

static void update(void)
{
	if (key_hit(KEY_B))
		scene_set(title_screen);

	if (key_hit(KEY_UP) || key_hit(KEY_DOWN))
	{
		_data->fx_mode = wrap(_data->fx_mode + key_tri_vert(), 0, ST_SELECTION_COUNT);
		switch (_data->fx_mode)
		{
		case ST_SELECTION_FX:
			obj_set_pos(_data->selection_arrow, 5, 35);
			break;
		case ST_SELECTION_MUSIC:
			obj_set_pos(_data->selection_arrow, 5, 80);
			break;
		default:
			break;
		}
	}

	if (key_hit(KEY_LEFT) || key_hit(KEY_RIGHT))
	{
		switch (_data->fx_mode)
		{
		case ST_SELECTION_FX:
			_data->fx_idx = wrap(_data->fx_idx + key_tri_horz(), 0, SOUND_SET_LENGTH);

			if (key_hit(KEY_LEFT))
				_data->sound_fx_left_arrow->attr2 = ATTR2_ID(stArrowTilesLen / 32);
			else
				_data->sound_fx_right_arrow->attr2 = ATTR2_ID(stArrowTilesLen / 32);
			break;
		case ST_SELECTION_MUSIC:
			_data->mus_idx = wrap(_data->mus_idx + key_tri_horz(), 0, MSL_NSONGS);

			if (key_hit(KEY_LEFT))
				_data->sound_mus_left_arrow->attr2 = ATTR2_ID(stArrowTilesLen / 32);
			else
				_data->sound_mus_right_arrow->attr2 = ATTR2_ID(stArrowTilesLen / 32);
			break;
		default:
			break;
		}

		_data->arrow_countdown = 10;
	}

	//Underflow fuck it
	_data->arrow_countdown--;
	if (_data->arrow_countdown < 0)
	{
		_data->sound_fx_left_arrow->attr2 = ATTR2_ID(0);
		_data->sound_fx_right_arrow->attr2 = ATTR2_ID(0);
		_data->sound_mus_left_arrow->attr2 = ATTR2_ID(0);
		_data->sound_mus_right_arrow->attr2 = ATTR2_ID(0);
	}

	if (key_hit(KEY_A))
	{
		mmStop();
		mmEffectCancelAll();
		switch (_data->fx_mode)
		{
		case ST_SELECTION_FX:
			mmEffectEx(_sound_fx_set[_data->fx_idx].sound);
			break;
		case ST_SELECTION_MUSIC:
			mmSetModuleVolume(300);
			mmStart(_data->mus_idx, MM_PLAY_ONCE);
			break;

		default:
			break;
		}
	}

	obj_hide_multi(_data->objs + 6, ST_OBJ_COUNT - 6);

	int offset = 7;

	//Fx Idx
	char str[50];
	sprintf(str, "%02d", _data->fx_idx);
	obj_puts2(
		105, 37, str,
		0xF200, _data->objs + offset);

	offset += strlen(str);

	//Fx Sound ID
	sprintf(str, "id:%d", _sound_fx_set[_data->fx_idx].sound->id);
	obj_puts2(
		140, 35, str,
		0xF200, _data->objs + offset);
	offset += strlen(str);

	//Fx Title
	obj_puts2(
		25, 35, "Fx:\0",
		0xF200, _data->objs + offset);
	offset += 3;

	//Fx Name
	obj_puts2(
		30, 55, _sound_fx_set[_data->fx_idx].name,
		0xF200, _data->objs + offset);
	offset += strlen(_sound_fx_set[_data->fx_idx].name);

	//Music text

	//Music Idx
	sprintf(str, "%02d", _data->mus_idx);
	obj_puts2(
		105, 80, str,
		0xF200, _data->objs + offset);
	offset += strlen(str);

	//Fx Title
	obj_puts2(
		25, 80, "Music:\0",
		0xF200, _data->objs + offset);
	offset += 3;

	obj_copy(oam_mem, _data->objs, ST_OBJ_COUNT);
}

static void hide(void)
{
	REG_DISPCNT = 0;
	obj_hide_multi(oam_mem, 128);
	mmStop();
}

const scene_t sound_test_scene = {
	.show = show,
	.update = update,
	.hide = hide};
