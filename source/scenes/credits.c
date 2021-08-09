#include "credits.h"

#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

#include "title_screen.h"
#include "scene_shared.h"

#include "../debug.h"
#include "../anime.h"
#include "../graphics.h"

#include "../assets/crPaulFace.h"
#include "../assets/crSpace.h"
#include "../assets/crPatrickFace.h"
#include "../assets/crWhaleLargeStanding.h"
#include "../assets/crBiscut.h"
#include "../assets/crPaulMic.h"
#include "../assets/crBuildings.h"
#include "../assets/crRealBuildings.h"
#include "../assets/crTunaPasta.h"
#include "../assets/crEmpty.h"
#include "../assets/whale_air_0.h"
#include "../assets/whale_air_1.h"
#include "../assets/whale_air_2.h"
#include "../assets/spriteShared.h"

static const int whale_air_anime_count = 3;
static const uint *whale_air_anime[] = {
	whale_air_0Tiles, whale_air_1Tiles, whale_air_2Tiles};

static const int star_pal_cycle_length = 4;
static const u16 star_pal_cycle[] = {0x77bd, 0x77ff, 0x7bff, 0x7fff};

static const int _credits_count = 11;
static const cr_credit_t _credits[] = {
	{
		"Programming:\n\tPaul Sarda\n\n"
		"Art:\n\tPaul Sarda\n\n\0",
		crPaulFacePal,
		crPaulFaceTiles,
		crPaulFaceMap,
	},
	{
		"Music:\nPatrick Dyett\0",
		crPatrickFacePal,
		crPatrickFaceTiles,
		crPatrickFaceMap,
	},
	{
		"Sound Effects:\n\n"
		"Patrick Dyett:\n\tBiscut Death\n\n"
		"harvey656/Labbed:\n\tWhale Walk\n\n"
		"JD wasabi:\n\tSpeed Token\0",
		crWhaleLargeStandingPal,
		crWhaleLargeStandingTiles,
		crWhaleLargeStandingMap,
	},
	{
		"More Sounds:\n\n"
		"TerraZoo:\n\tCroc Growl\n\n"
		"Eternity Games:\n\tAlways Backwards\n\n",
		crEmptyPal,
		crEmptyTiles,
		crEmptyMap,
	},
	{
		"More Sounds:\n\n"
		"Paul Sarda:\n"
		"\tUFO Biscut Death\n"
		"\tVoice Lines\n"
		"\tPlayer Flap\n\n",
		crPaulMicPal,
		crPaulMicTiles,
		crPaulMicMap,
	},
	{
		"More Sounds:\n\n"
		"Beep Yeah:\n"
		"\tJump Token\n"
		"\tHealth Token\n"
		"\tPlayer Shoot\n"
		"\tPlayer Jump\n"
		"\tPlayer Land\n"
		"\tPlayer Shrink\n"
		"\tEnemy Shoot\n",
		crBiscutPal,
		crBiscutTiles,
		crBiscutMap,
	},
	{
		"Assets by Beep Yeah\n"
		"are from\n"
		"8-Bit Sound Effects\n[100+ SFX] pack\n"
		"and has no direct\n"
		"association with this\n"
		"game\0",
		crBuildingsPal,
		crBuildingsTiles,
		crBuildingsMap,
	},
	{
		"Assets by JD wasabi\n"
		"are from\n"
		"8-bit / 16-bit\nSound Effects\n(x25) Pack\n"
		"and has no direct\n"
		"association with this\n"
		"game\0",
		crRealBuildingsPal,
		crRealBuildingsTiles,
		crRealBuildingsMap,
	},
	{
		"Assets by\n"
		"harvey656/Labbed\n"
		"are from\n"
		"8-bit Game Sound\nEffects Collection\n"
		"and has no direct\n"
		"association with this\n"
		"game\0",
		crEmptyPal,
		crEmptyTiles,
		crEmptyMap,
	},
	{
		"Assets by\n"
		"TerraZoo\n"
		"are from\n"
		"Reptiles and more\n (SOE009)\n"
		"and has no direct\n"
		"association with this\n"
		"game\0",
		crEmptyPal,
		crEmptyTiles,
		crEmptyMap,
	},
	{
		"Assets by\n"
		"Eternity Games\n"
		"are from\n"
		"Old Man WAV\n"
		"and has no direct\n"
		"association with this\n"
		"game\0",
		crEmptyPal,
		crEmptyTiles,
		crEmptyMap,
	},

};

static cr_data_t *_data = &_shared_data.cr;

static void load_next_credit()
{
	_data->eva = 0 * FIX_SCALE;
	_data->evb = 128 * FIX_SCALE;

	_data->active.crd = &_credits[_data->active_idx];
	_data->active.x = 50 * FIX_SCALE;
	_data->active.y = GBA_HEIGHT * FIX_SCALE;
	_data->state = CR_STATE_SCROLLING_UP;

	//Load pal
	LZ77UnCompVram(_data->active.crd->pal, pal_bg_mem);
	//Face background
	LZ77UnCompVram(_data->active.crd->tiles, &tile8_mem[CR_SHARED_CB]);
	//Map
	LZ77UnCompVram(_data->active.crd->map, &se_mem[CR_IMAGE_SBB]);

	_data->active_idx++;
}

static void show(void)
{
	irq_init(NULL);
	irq_add(II_VBLANK, mmVBlank);

	/* Load palettes */
	GRIT_CPY(pal_bg_mem + CR_SKY_START_PAL, crSpacePal);
	/* Load background tiles into CR_SHARED_CB Note the image background is loaded on the fly */
	//Space
	//Fuckyness since we don't have a combined pallete
	unsigned char crSpaceTilesTmp[sizeof(crSpaceTiles) / sizeof(unsigned char)];
	for (int i = 0; i < sizeof(crSpaceTiles) / sizeof(unsigned char); i++)
		crSpaceTilesTmp[i] = crSpaceTiles[i] + 120;
	dma3_cpy(&tile8_mem[CR_SHARED_CB][600], crSpaceTilesTmp, crSpaceTilesLen);

	// Background maps
	GRIT_CPY(&se_mem[CR_SPACE_SSB], crSpaceMap);

	//Set offsets
	REG_BG0HOFS = 0;
	REG_BG0VOFS = 0;

	REG_BG1HOFS = 0;
	REG_BG1VOFS = 0;

	REG_BG2HOFS = 0;
	REG_BG2VOFS = 0;

	//Set bkg reg
	REG_BG0CNT = BG_PRIO(1) | BG_8BPP | BG_SBB(CR_SPACE_SSB) | BG_CBB(CR_SHARED_CB) | BG_REG_32x32;
	REG_BG1CNT = BG_PRIO(0) | BG_8BPP | BG_SBB(CR_IMAGE_SBB) | BG_CBB(CR_SHARED_CB) | BG_REG_32x32;
	REG_BG2CNT = BG_PRIO(2) | BG_SBB(CR_TEXT_SBB) | BG_CBB(CR_SHARED_CB) | BG_REG_32x32;

	//Display Reg
	REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D | DCNT_BG0 | DCNT_BG1 | DCNT_BG2;

	REG_BLDCNT = BLD_BUILD(
		BLD_BG1, // Top layers
		BLD_BG0, // Bottom layers
		1		 // Mode
	);

	// clear the screenblock I'm about to use
	memset32(&se_mem[CR_TEXT_SBB], 0, SBB_SIZE / 4);

	// init map text
	txt_init_std();
	txt_init_obj(oam_mem, 0xF200, CLR_YELLOW, 0x0E);
	// 12 px between letters
	gptxt->dx = 7;
	gptxt->dy = 10;

	_data->active_idx = 0;

	load_next_credit();

	//init objects
	obj_hide_multi(_data->text_objs, CR_OBJ_COUNT);

	_data->player = &_data->text_objs[CR_OBJ_COUNT - 1];

	GRIT_CPY(pal_obj_mem, spriteSharedPal);
	GRIT_CPY(tile_mem[4], whale_air_0Tiles);
	obj_set_attr(
		_data->player,
		ATTR0_SQUARE | ATTR0_8BPP,
		ATTR1_SIZE_16,
		ATTR2_ID(0) | ATTR2_PRIO(0));
	_data->player_x = (GBA_WIDTH / 2 - 8) * FIX_SCALE;
	_data->player_y = (GBA_HEIGHT / 2 - 8) * FIX_SCALE;
	obj_set_pos(_data->player, fx2int(_data->player_x), fx2int(_data->player_y));

	//Setup blending
	_data->eva = 3 * FIX_SCALE;
	_data->evb = 124 * FIX_SCALE;

	mmStart(MOD_PD_ROCK_BACKGROUND, MM_PLAY_LOOP);
}

static int longest_word_in_sentance(const char *str)
{
	int best = 0;
	int current = 0;
	while (*str != '\0')
	{
		if (*str != '\n')
		{
			++current;
		}
		else
		{
			if (current > best)
			{
				best = current;
			}
			current = 0;
		}

		++str;
	}

	return best;
}

static void update(void)
{
	REG_BLDALPHA = BLDA_BUILD(fx2int(_data->eva) / 8, fx2int(_data->evb) / 8);
	REG_BLDY = BLDY_BUILD(0);

	if (key_hit(KEY_START) || key_hit(KEY_B))
		scene_set(title_screen);

	FIXED step;
	switch (_data->state)
	{
	case CR_STATE_SCROLLING_UP:
		_data->active.y -= CR_TEXT_SCROLL_SPEED;

		step = fxdiv(130, CR_FADE_TIME);
		_data->evb = clamp(_data->evb - step, 0, 124 * FIX_SCALE);

		if (fx2int(_data->active.y) < GBA_HEIGHT / 4)
		{
			_data->state = CR_STATE_PAUSE;
			_data->countdown = CR_PAUSE_TIME;
		}
		break;
	case CR_STATE_PAUSE:
		--_data->countdown;
		step = fxdiv(130, CR_PAUSE_TIME);
		_data->eva = clamp(_data->eva + step, 0, 124 * FIX_SCALE);

		if (_data->countdown <= 0)
		{
			_data->state = CR_STATE_LEAVE;
			_data->countdown = 60;
		}
		break;
	case CR_STATE_LEAVE:
	{
		_data->countdown--;
		int compelte;
		if (_data->active_idx % 2 == 0)
		{
			_data->active.x -= CR_TEXT_SCROLL_SPEED * 2;
			compelte = fx2int(_data->active.x) + 12 * longest_word_in_sentance(_data->active.crd->str) <= 0;
		}
		else
		{
			_data->active.x += CR_TEXT_SCROLL_SPEED * 2;
			compelte = fx2int(_data->active.x) >= GBA_WIDTH;
		}

		if (_data->countdown < 0)
		{
			step = fxdiv(130, CR_FADE_TIME);
			_data->evb = clamp(_data->evb + step, 0, 124 * FIX_SCALE);
			step = fxdiv(130, CR_FADE_TIME);
			_data->eva = clamp(_data->eva - step, 0, 124 * FIX_SCALE);
		}

		if (compelte)
		{
			if (_data->active_idx < _credits_count)
				load_next_credit();
			else
				scene_set(title_screen);
		}
		break;
	}
	}

	//Cycle star colours
	if (frame_count() % 35 == 0)
	{
		cycle_palate(
			pal_bg_mem,
			CR_SKY_START_PAL + 2, star_pal_cycle,
			&_data->star_pal_idx, star_pal_cycle_length);
	}

	//Player movement
	if (frame_count() % 4 == 0)
	{
		step_anime(
			&_data->player_anime_cycle, whale_air_anime,
			whale_air_anime_count,
			0, whale_air_0TilesLen);
	}

	if (key_hit(KEY_LEFT))
		_data->player->attr1 = ATTR1_SIZE_16 | ATTR1_HFLIP;
	else if (key_hit(KEY_RIGHT))
		_data->player->attr1 = ATTR1_SIZE_16;

	_data->player_x = wrap(_data->player_x + int2fx(key_tri_horz()), -16 * FIX_SCALE, GBA_WIDTH * FIX_SCALE);
	_data->player_y = wrap(_data->player_y + int2fx(key_tri_vert()), -16 * FIX_SCALE, GBA_HEIGHT * FIX_SCALE);
	obj_set_pos(_data->player, fx2int(_data->player_x), fx2int(_data->player_y));

	//Draws text
	obj_puts2(
		fx2int(_data->active.x), fx2int(_data->active.y),
		_data->active.crd->str, 0xF200, _data->text_objs);
	oam_copy(oam_mem, _data->text_objs, CR_OBJ_COUNT);
}

static void hide(void)
{
	REG_BLDCNT = 0;

	mmStop();

	//init objects
	obj_hide_multi(_data->text_objs, CR_OBJ_COUNT);
	oam_copy(oam_mem, _data->text_objs, CR_OBJ_COUNT);
}

const scene_t credits_screen = {
	.show = show,
	.update = update,
	.hide = hide};
