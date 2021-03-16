#include "game_intro.h"
#include <stdio.h>

#include <tonc.h>
#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

#include "main_game.h"
#include "../graphics.h"
#include "../debug.h"

#include "../assets/ready.h"
#include "../assets/set.h"
#include "../assets/go.h"
#include "../assets/giBackgroundShared.h"
#include "../assets/giSpriteShared.h"
#include "../assets/whaleLarge.h"
#include "../assets/giCityTop.h"
#include "../assets/giSky.h"
#include "../assets/giBackgroundAffShared.h"

static const int shared_cb = 0;
static const int cool_background_ssb = 8;
static const int text_ssb = 24;
static const int sky_ssb = 26;
static const mm_sfxhand _intro_handler = 1;
static const M7_D = 128;
static const PHI0 = 0;
static const whale_att = 0;

static VECTOR cam_pos;		 // Camera position
static FIXED g_cosf, g_sinf; // cos(phi) and sin(phi), .8f
static gi_states_t _state;
static u16 cam_phi = PHI0;
static OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE *)_obj_buffer;
static int whale_scale;
static int whale_rotate;
static int countdown;

// --- Type A ---
// By the numbers: everything nice and .8 fixed point. Result: blocky
// - (offset * zoom) * rotate
// - lambda is .8f for xs and ys
// - M7_D multiplied before shift
// --- Type C ---
// offset * (zoom * rotation)
// Mixed fixed point: lam, lxr, lyr use .12
// lxr and lyr have different calculation methods
//Not going to pretend that I understand this
static void m7_hbl()
{
	FIXED lam, lcf, lsf, lxr, lyr;

	lam = cam_pos.y * lu_div(REG_VCOUNT) >> 12; // .8*.16 /.12 = 20.12
	lcf = lam * g_cosf >> 8;					// .12*.8 /.8 = .12
	lsf = lam * g_sinf >> 8;					// .12*.8 /.8 = .12

	REG_BG2PA = lcf >> 4;
	REG_BG2PC = lsf >> 4;

	// Offsets
	// Note that the lxr shifts down first!

	// horizontal offset
	lxr = 120 * (lcf >> 4);
	lyr = (M7_D * lsf) >> 4;
	REG_BG2X = cam_pos.x - lxr + lyr;

	// vertical offset
	lxr = 120 * (lsf >> 4);
	lyr = (M7_D * lcf) >> 4;
	REG_BG2Y = cam_pos.z - lxr - lyr;
}

static void show(void)
{
	// Set RegX scroll to 0
	REG_BG1HOFS = 0;
	REG_BG1VOFS = 0;

	/* Load palettes */
	GRIT_CPY(pal_bg_mem + 240, giBackgroundSharedPal);
	GRIT_CPY(pal_bg_mem, giBackgroundAffSharedPal);
	GRIT_CPY(pal_obj_mem, giSpriteSharedPal);
	/* Load background tiles into shared_cb */
	//afine background
	GRIT_CPY(&tile_mem[shared_cb], giBackgroundAffSharedTiles);
	//reg background
	GRIT_CPY(&tile_mem[shared_cb][450 * 2], giBackgroundSharedTiles);
	//Load object tiles
	GRIT_CPY(&tile_mem[4][0], whaleLargeTiles);

	// Background maps
	GRIT_CPY(&se_mem[cool_background_ssb], giCityTopMap);
	GRIT_CPY(&se_mem[text_ssb], readyMap);
	GRIT_CPY(&se_mem[sky_ssb], giSkyMap);

	REG_BG0CNT = BG_PRIO(2) | BG_8BPP | BG_SBB(sky_ssb) | BG_CBB(shared_cb) | BG_REG_32x32;
	REG_BG1CNT = BG_PRIO(0) | BG_8BPP | BG_SBB(text_ssb) | BG_CBB(shared_cb) | BG_REG_32x32;
	REG_BG2CNT = BG_PRIO(1) | BG_SBB(cool_background_ssb) | BG_CBB(shared_cb) | BG_AFF_64x64;

	// enable hblank register and set the mode7 type
	irq_init(NULL);
	irq_add(II_HBLANK, NULL);
	// // and vblank int for vsync
	irq_add(II_VBLANK, mmVBlank);
	irq_add(II_HBLANK, m7_hbl);

	REG_DISPCNT = DCNT_MODE1 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;

	// Setup cam
	cam_pos.x = 257.89f * FIX_SCALEF;
	cam_pos.y = 200.0f * FIX_SCALEF;
	cam_pos.z = 496.38f * FIX_SCALEF;
	cam_phi = 0;
	g_cosf = lu_cos(cam_phi) >> 4;
	g_sinf = lu_sin(cam_phi) >> 4;

	//setup whale
	obj_set_attr(
		&_obj_buffer[0],
		ATTR0_SQUARE | ATTR0_8BPP | ATTR0_AFF | ATTR0_AFF_DBL_BIT,
		ATTR1_SIZE_64x64,
		ATTR2_PRIO(1) | ATTR1_AFF_ID(0));

	obj_set_pos(&_obj_buffer[0], GBA_WIDTH / 2 - 64, 15);
	obj_aff_identity(&obj_aff_buffer[0]);

	//Start sound
	_state = GI_S_READY;

	mm_sound_effect shoot_sound = {
		{SFX_READY_0},
		(int)(1.0f * (1 << 10)),
		_intro_handler,
		120,
		127,
	};
	mmEffectEx(&shoot_sound);

	whale_scale = 512;
	whale_rotate = 128;
}

static void update(void)
{
	cam_pos.y -= 1.8f * FIX_SCALE;
	whale_scale -= 5;
	whale_rotate += 1024;

	obj_aff_rotate_inv(&obj_aff_buffer[0], whale_rotate);
	obj_aff_scale_inv(&obj_aff_buffer[0], whale_scale, whale_scale);
	obj_copy(obj_mem, _obj_buffer, 2);
	obj_aff_copy(obj_aff_mem, obj_aff_buffer, 3);

	switch (_state)
	{
	case GI_S_READY:
	{
		if (mmEffectActive(_intro_handler))
			return;

		mm_sound_effect set_sound = {
			{SFX_SET_0},
			(int)(1.0f * (1 << 10)),
			_intro_handler,
			120,
			127,
		};
		mmEffectEx(&set_sound);
		_state = GI_S_SET;
		GRIT_CPY(&se_mem[text_ssb], setMap);
		break;
	}
	case GI_S_SET:
	{
		if (mmEffectActive(_intro_handler))
			return;

		GRIT_CPY(&se_mem[text_ssb], goMap);
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
		//1/2 a second
		countdown = 30;
		break;
	}
	case GI_S_GO:
		countdown--;
		if (countdown <= 0)
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
