#include "credits.h"

#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

#include "title_screen.h"
#include "scene_shared.h"

#include "../debug.h"

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

static const int creditsCount = 8;
static const cr_credit_t credits[] = {
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
};

static cr_data_t *data = &_shared_data.cr;

static void load_next_credit()
{
	data->eva = 0 * FIX_SCALE;
	data->evb = 128 * FIX_SCALE;

	data->active.crd = &credits[data->active_idx];
	data->active.x = 50 * FIX_SCALE;
	data->active.y = GBA_HEIGHT * FIX_SCALE;
	data->state = CR_STATE_SCROLLING_UP;

	//Load pal
	LZ77UnCompVram(data->active.crd->pal, pal_bg_mem);
	//Face background
	LZ77UnCompVram(data->active.crd->tiles, &tile8_mem[CR_SHARED_CB]);
	//Map
	LZ77UnCompVram(data->active.crd->map, &se_mem[CR_IMAGE_SBB]);

	data->active_idx++;
}

static void show(void)
{
	irq_init(NULL);
	irq_add(II_VBLANK, mmVBlank);

	/* Load palettes */
	GRIT_CPY(pal_bg_mem + 120, crSpacePal);
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
	REG_DISPCNT = DCNT_OBJ | DCNT_BG0 | DCNT_BG1 | DCNT_BG2;

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

	data->active_idx = 0;

	load_next_credit();

	for (int i = 0; i < 128; i++)
	{
		data->text_objs[i].attr0 = ATTR0_HIDE;
	}

	data->eva = 3 * FIX_SCALE;
	data->evb = 124 * FIX_SCALE;

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
	REG_BLDALPHA = BLDA_BUILD(fx2int(data->eva) / 8, fx2int(data->evb) / 8);
	REG_BLDY = BLDY_BUILD(0);

	FIXED step;
	switch (data->state)
	{
	case CR_STATE_SCROLLING_UP:
		data->active.y -= CR_TEXT_SCROLL_SPEED;

		step = fxdiv(124, CR_FADE_TIME);
		data->evb = clamp(data->evb - step, 3 * FIX_SCALE, 124 * FIX_SCALE);

		if (fx2int(data->active.y) < GBA_HEIGHT / 4)
		{
			data->state = CR_STATE_PAUSE;
			data->countdown = CR_PAUSE_TIME;
		}
		break;
	case CR_STATE_PAUSE:
		--data->countdown;
		step = fxdiv(124, CR_PAUSE_TIME);
		data->eva = clamp(data->eva + step, 3 * FIX_SCALE, 124 * FIX_SCALE);

		if (data->countdown <= 0)
		{
			data->state = CR_STATE_LEAVE;
			data->countdown = 60;
		}
		break;
	case CR_STATE_LEAVE:
	{
		data->countdown--;
		int compelte;
		if (data->active_idx % 2 == 0)
		{
			data->active.x -= CR_TEXT_SCROLL_SPEED * 2;
			compelte = fx2int(data->active.x) + 12 * longest_word_in_sentance(data->active.crd->str) <= 0;
		}
		else
		{
			data->active.x += CR_TEXT_SCROLL_SPEED * 2;
			compelte = fx2int(data->active.x) >= GBA_WIDTH;
		}

		if (data->countdown < 0)
		{
			step = fxdiv(124, CR_FADE_TIME);
			data->evb = clamp(data->evb + step, 3 * FIX_SCALE, 124 * FIX_SCALE);
			step = fxdiv(124, CR_FADE_TIME);
			data->eva = clamp(data->eva - step, 3 * FIX_SCALE, 124 * FIX_SCALE);
		}

		if (compelte)
		{
			if (data->active_idx < creditsCount)
				load_next_credit();
			else
				scene_set(title_screen);
		}
		break;
	}
	}

	//Draws text
	obj_puts2(
		fx2int(data->active.x), fx2int(data->active.y),
		data->active.crd->str, 0xF200, data->text_objs);
	oam_copy(oam_mem, data->text_objs, 128);
}

static void hide(void)
{
	mmStop();

	OAM_CLEAR();
}

const scene_t credits_screen = {
	.show = show,
	.update = update,
	.hide = hide};
