#include "game_intro.h"
#include <stdio.h>

#include <tonc.h>
#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

#include "main_game.h"
#include "special_zone.h"
#include "scene_shared.h"
#include "../graphics.h"
#include "../debug.h"
#include "../anime.h"
#include "../sound.h"

#include "../assets/ready.h"
#include "../assets/set.h"
#include "../assets/go.h"
#include "../assets/giBackgroundShared.h"
#include "../assets/giSpriteShared.h"
#include "../assets/giCityTop.h"
#include "../assets/giSky.h"
#include "../assets/giBeachAffShared.h"
#include "../assets/giCityAffShared.h"
#include "../assets/giBeachTop.h"
#include "../assets/giWhale_air_0.h"
#include "../assets/giWhale_air_1.h"
#include "../assets/giWhale_air_2.h"
#include "../assets/giWhale_air_3.h"
#include "../assets/giSZTop.h"
#include "../assets/giSZAffShared.h"

static const uint *air_anime_cycle[] = {giWhale_air_0Tiles, giWhale_air_0Tiles, giWhale_air_1Tiles, giWhale_air_3Tiles};

static gi_data_t *_data = &_shared_data.gi;

// --- Type A ---
// By the numbers: everything nice and .8 fixed point. Result: blocky
// - (offset * zoom) * rotate
// - lambda is .8f for xs and ys
// - M7_D multiplied before shift
// --- Type C ---
// offset * (zoom * rotation)
// Mixed fixed point: lam, lxr, lyr use .12
// lxr and lyr have different calculation methods
// Not going to pretend that I understand this
static void m7_hbl()
{
	FIXED lam, lcf, lsf, lxr, lyr;

	lam = _data->cam_pos.y * lu_div(REG_VCOUNT) >> 12; // .8*.16 /.12 = 20.12
	lcf = lam * _data->g_cosf >> 8;					   // .12*.8 /.8 = .12
	lsf = lam * _data->g_sinf >> 8;					   // .12*.8 /.8 = .12

	REG_BG2PA = lcf >> 4;
	REG_BG2PC = lsf >> 4;

	// Offsets
	// Note that the lxr shifts down first!

	// horizontal offset
	lxr = 120 * (lcf >> 4);
	lyr = (GI_M7_D * lsf) >> 4;
	REG_BG2X = _data->cam_pos.x - lxr + lyr;

	// vertical offset
	lxr = 120 * (lsf >> 4);
	lyr = (GI_M7_D * lcf) >> 4;
	REG_BG2Y = _data->cam_pos.z - lxr - lyr;
}

static void show(gi_mode_t mode)
{
	_data->mode = mode;

	_data->obj_aff_buffer = (OBJ_AFFINE *)_obj_buffer;

	// Set RegX scroll to 0
	REG_BG1HOFS = 0;
	REG_BG1VOFS = 0;

	/* Load palettes */
	GRIT_CPY(pal_obj_mem, giSpriteSharedPal);
	// Load background tiles into GI_SHARED_CB
	// reg background
	GRIT_CPY(&tile_mem[GI_SHARED_CB][450 * 2], giBackgroundSharedTiles);
	// Load object tiles
	GRIT_CPY(&tile_mem[4][0], giWhale_air_0Tiles);

	// afine background
	switch (mode)
	{
	case GI_MODE_BEACH:
		GRIT_CPY(pal_bg_mem, giBeachAffSharedPal);
		//The fucking grit is being a stupid fucking bitch fucking asshole
		pal_bg_mem[7] = 0x76eb;
		LZ77UnCompVram(giBeachAffSharedTiles, &tile_mem[GI_SHARED_CB]);
		GRIT_CPY(&se_mem[GI_COOL_BACKGROUND_SSB], giBeachTopMap);
		break;

	case GI_MODE_CITY:
		GRIT_CPY(pal_bg_mem, giCityAffSharedPal);
		LZ77UnCompVram(giCityAffSharedTiles, &tile_mem[GI_SHARED_CB]);
		GRIT_CPY(&se_mem[GI_COOL_BACKGROUND_SSB], giCityTopMap);
		break;
	case GI_MODE_SPEICAL_ZONE:
		// Random paltte
		for (int i = 0; i < giSZAffSharedPalLen; i++)
			pal_bg_mem[i] = (u16)gba_rand();
		LZ77UnCompVram(giSZAffSharedTiles, &tile_mem[GI_SHARED_CB]);
		GRIT_CPY(&se_mem[GI_COOL_BACKGROUND_SSB], giSZTopMap);
		break;
	}
	GRIT_CPY(&se_mem[GI_TEXT_SSB], readyMap);
	GRIT_CPY(&se_mem[GI_SKY_SSB], giSkyMap);

	REG_BG0CNT = BG_PRIO(2) | BG_8BPP | BG_SBB(GI_SKY_SSB) | BG_CBB(GI_SHARED_CB) | BG_REG_32x32;
	REG_BG1CNT = BG_PRIO(0) | BG_8BPP | BG_SBB(GI_TEXT_SSB) | BG_CBB(GI_SHARED_CB) | BG_REG_32x32;
	REG_BG2CNT = BG_PRIO(1) | BG_SBB(GI_COOL_BACKGROUND_SSB) | BG_CBB(GI_SHARED_CB) | BG_AFF_64x64 | BG_WRAP;

	// enable hblank register and set the mode7 type
	irq_init(NULL);
	irq_add(II_HBLANK, NULL);
	// // and vblank int for vsync
	irq_add(II_HBLANK, NULL);
	irq_add(II_VBLANK, mmVBlank);
	irq_add(II_HBLANK, m7_hbl);

	REG_DISPCNT = DCNT_MODE1 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;

	// Setup cam
	_data->cam_pos.x = gba_rand(); // 257.89f * FIX_SCALEF
	_data->cam_pos.y = 600.0f * FIX_SCALEF;
	_data->cam_pos.z = gba_rand();
	_data->cam_phi = gba_rand();
	_data->g_cosf = lu_cos(_data->cam_phi) >> 4;
	_data->g_sinf = lu_sin(_data->cam_phi) >> 4;

	// setup whale
	obj_set_attr(
		&_obj_buffer[0],
		ATTR0_SQUARE | ATTR0_8BPP | ATTR0_AFF | ATTR0_AFF_DBL_BIT,
		ATTR1_SIZE_64x64 | ATTR1_AFF_ID(0),
		ATTR2_PRIO(1));

	obj_set_pos(&_obj_buffer[0], GBA_WIDTH / 2 - 64, GBA_HEIGHT / 2 - 64);
	obj_aff_identity(&_data->obj_aff_buffer[0]);

	// start sound
	_data->state = GI_S_READY;

	if (_data->mode == GI_MODE_SPEICAL_ZONE)
		mmStart(MOD_PD_SPECIAL_ZONE, MM_PLAY_LOOP);

	mmEffectEx(&_ready_sound);

	_data->countdown = GI_STARTING_COUNTDOWN;
	_data->whale_scale = GI_WHALE_START_SCALE;
	_data->whale_rotate = 0;
	_data->anime_cycle = 3;
}

static void update(void)
{
	step_anime_bad(
		air_anime_cycle, giWhale_air_0TilesLen, 3,
		&_data->anime_cycle, 0);

	--_data->countdown;
	// This is a magic number that makes it roate fast
	_data->whale_rotate -= 128 * 5;
	_data->cam_pos.y -= 5.5f * FIX_SCALE;
	_data->whale_scale -= fxdiv(GI_WHALE_START_SCALE, GI_STARTING_COUNTDOWN * FIX_SCALE);

	obj_aff_rotscale(
		&_data->obj_aff_buffer[0],
		// This fucky math inverts it
		((1 << 24) / fx2int(_data->whale_scale)) >> 8,
		((1 << 24) / fx2int(_data->whale_scale)) >> 8,
		_data->whale_rotate);

	obj_copy(obj_mem, _obj_buffer, 1);
	obj_aff_copy(obj_aff_mem, _data->obj_aff_buffer, 1);

	switch (_data->state)
	{
	case GI_S_READY:
	{
		if (mmEffectActive(GI_INTRO_HANDLER))
			return;

		mmEffectEx(&_set_sound);
		_data->state = GI_S_SET;
		GRIT_CPY(&se_mem[GI_TEXT_SSB], setMap);
		break;
	}
	case GI_S_SET:
	{
		if (mmEffectActive(GI_INTRO_HANDLER))
			return;

		GRIT_CPY(&se_mem[GI_TEXT_SSB], goMap);
		mmEffectEx(&_go_sound);
		mmSetModuleVolume(300);
		switch (_data->mode)
		{
		case GI_MODE_BEACH:
		case GI_MODE_CITY:
			mmStart(MOD_INTRO, MM_PLAY_ONCE);
			break;
		case GI_MODE_SPEICAL_ZONE:
			break;
		}
		_data->state = GI_S_GO;
		break;
	}
	case GI_S_GO:
		if (_data->cam_pos.y <= 50 * FIX_SCALE)
		{
			switch (_data->mode)
			{
			case GI_MODE_BEACH:
			case GI_MODE_CITY:
				scene_set(_main_game_scene);
				break;
			case GI_MODE_SPEICAL_ZONE:
				scene_set(_special_zone_scene);
				break;
			}
		}
	}
}

static void hide(void)
{
	REG_DISPCNT = 0;
}

static void show_city(void)
{
	show(GI_MODE_CITY);
}

static void show_beach(void)
{
	show(GI_MODE_BEACH);
}

static void show_speical_zone(void)
{
	show(GI_MODE_SPEICAL_ZONE);
}

const scene_t _city_game_intro_scene = {
	.show = show_city,
	.update = update,
	.hide = hide};

const scene_t _beach_game_intro_scene = {
	.show = show_beach,
	.update = update,
	.hide = hide};

const scene_t _speical_zone_intro_scene = {
	.show = show_speical_zone,
	.update = update,
	.hide = hide};