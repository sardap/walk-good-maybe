#include "main_game.h"

#include <tonc.h>

#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

#include "title_screen.h"
#include "game_over.h"
#include "scene_shared.h"
#include "special_zone.h"
#include "game_win.h"
#include "../ent.h"
#include "../player.h"
#include "../graphics.h"
#include "../level.h"
#include "../debug.h"
#include "../numbers.h"
#include "../gun.h"
#include "../enemy.h"
#include "../ui_display.h"
#include "../gen.h"
#include "../obstacles.h"
#include "../effect.h"
#include "../anime.h"
#include "../sound.h"

#include "../assets/backgroundCity.h"
#include "../assets/fog.h"
#include "../assets/mainGameCityShared.h"
#include "../assets/mainGameBeachShared.h"
#include "../assets/buildingtileset.h"
#include "../assets/mgBeach.h"
#include "../assets/mgPauseBeach.h"
#include "../assets/mgPauseCity.h"
#include "../assets/mgBeachWaterFog00.h"
#include "../assets/mgBeachWaterFog01.h"
#include "../assets/spriteShared.h"

static const int fog_water_cycle_length = 2;
static const tile_map_set_t fog_water_cycle[] = {
	{mgBeachWaterFog00Tiles, mgBeachWaterFog00TilesLen,
	 mgBeachWaterFog00Map, mgBeachWaterFog00MapLen},
	{mgBeachWaterFog01Tiles, mgBeachWaterFog01TilesLen,
	 mgBeachWaterFog01Map, mgBeachWaterFog01MapLen}};

EWRAM_DATA static int _obj_pal_idx;

static mg_data_t *_data = &_shared_data.mg;

EWRAM_DATA static mg_data_in_t _in_data;
EWRAM_DATA static mg_data_out_t _out_data;

mg_data_in_t defualt_mg_data(mg_mode_t mode)
{
	mg_data_in_t result;
	mg_data_t *data = &result.new_data;

	data->mode = mode;

	data->bg_0_scroll = int2fx(gba_rand());
	data->bg_2_scroll = int2fx(gba_rand());

	data->next_building_spawn = 0;
	data->building_spawn_x = 0;
	data->state = MG_S_STARTING;
	data->water_pal_idx = 0;
	data->splash_active = 0;
	data->fog_tiles_cycle_idx = 0;
	data->starting_scroll_x = 0;

	data->fresh_game = TRUE;

	return result;
}

void set_mg_in(mg_data_in_t data)
{
	_in_data = data;
}

mg_data_out_t get_mg_out()
{
	return _out_data;
}

static FIXED wrap_x(FIXED x)
{
	if (x > MG_BG_X_PIX)
	{
		return x - MG_BG_X_PIX;
	}
	else if (x < 0)
	{
		return x - -x;
	}

	return x;
}

static inline int offset_x_bg(int n)
{
	// What the fuck is nfx
	int nfx = (n * TILE_WIDTH) * FIX_SCALE;
	return fx2int(wrap_x(_bg_pos_x + nfx)) / TILE_WIDTH;
}

static void wrap_bkg()
{
	_bg_pos_x = wrap_x(_bg_pos_x);
}

static void spawn_buildings()
{
	int start_x = _data->building_spawn_x;

	// Calcautes how much spare space there is
	int space = 0;
	// Yeah that's right you can hire me after reading this shit show
	while (tile_to_collision(at_level(level_wrap_x(start_x + space), SCREEN_HEIGHT / 8)) == LEVEL_COL_EMPTY)
	{
		space++;
	}

	int width = 0;
	switch (_data->mode)
	{
	case MG_MODE_CITY:
		switch (gba_rand_range(1, 6))
		{
		case 1:
			width = spawn_building_0(start_x);
			break;
		case 2:
			width = spawn_building_1(start_x);
			break;
		case 3:
			width = spawn_building_2(start_x);
			break;
		case 4:
			width = spawn_building_3(start_x);
			break;
		case 5:
			width = spawn_building_4(start_x);
			break;
		case 6:
			width = spawn_building_5(start_x);
			break;
		default:
			width = spawn_building_6(start_x);
			break;
		}
		break;
	case MG_MODE_BEACH:
		switch (gba_rand_range(1, 2))
		{
		case 1:
			width = spawn_island_00(start_x);
			break;
		case 2:
		default:
			width = spawn_island_01(start_x);
			break;
		}
		break;
	}

	width += gba_rand_range(MIN_JUMP_WIDTH_TILES, MAX_JUMP_WIDTH_TILES);

	_data->building_spawn_x = level_wrap_x(start_x + width);
	_data->next_building_spawn = int2fx(width * 8);

	space -= width;
	if (space > MAX_JUMP_WIDTH_TILES)
	{
		spawn_buildings();
	}
#ifdef DEBUG
	char str[50];
	sprintf(str, "Next building spawn %d spawn x %d", fx2int(_data->next_building_spawn), _data->building_spawn_x);
	write_to_log(LOG_LEVEL_DEBUG, str);
#endif
}

static void clear_offscreen_level()
{
	int x = level_wrap_x((fx2int(_bg_pos_x) / TILE_WIDTH) - 3);
	set_level_col(x, 0, 0);
}

static void load_foreground_tiles()
{
	// Order doesn't matter but all buildings / islands need to be loaded in a single chunk
	switch (_data->mode)
	{
	case MG_MODE_CITY:
		load_lava_0(MG_SHARED_CB);
		load_building_0(MG_SHARED_CB);
		load_building_1(MG_SHARED_CB);
		load_building_2(MG_SHARED_CB);
		load_building_3(MG_SHARED_CB);
		load_building_4(MG_SHARED_CB);
		load_building_5(MG_SHARED_CB);
		load_building_6(MG_SHARED_CB);
		break;
	case MG_MODE_BEACH:
		load_lava_0(MG_SHARED_CB);
		load_island_00(MG_SHARED_CB);
		load_island_01(MG_SHARED_CB);
		break;
	}
}

static void free_foreground_tiles()
{
	switch (_data->mode)
	{
	case MG_MODE_CITY:
		unload_lava_0();
		unload_building_0();
		unload_building_1();
		unload_building_2();
		unload_building_3();
		unload_building_4();
		free_building_5();
		free_building_6();
		break;
	case MG_MODE_BEACH:
		unload_lava_0();
		free_island_00();
		free_island_01();
		break;
	}
}

static void show(void)
{
	*_data = _in_data.new_data;

	// Apply speical zone bonuses
	sz_transfer_out_t sz_out = get_sz_out();
	if (sz_out.dirty)
	{
		sz_out_clear_dirty();

		for (int i = 0; i < sz_out.good_collected - sz_out.bad_collected; i++)
		{
			add_score(gba_rand_range(1, 10));

			if (gba_rand_range(0, 100) >= 80)
			{
#ifdef DEBUG
				write_to_log(LOG_LEVEL_DEBUG, "Lucky");
#endif
				if (gba_rand() % 2 == 0)
				{
					add_player_jump(fxdiv(PLAYER_ADD_JUMP_STEP, 2.5f * FIX_SCALEF));
				}
				else
				{
					add_player_speed(fxdiv(PLAYER_ADD_SPEED_STEP, 2.5f * FIX_SCALEF));
				}
			}
		}
	}

	load_blank();

	irq_init(NULL);
	irq_add(II_VBLANK, mmVBlank);

	if (_data->fresh_game)
	{
		free_all_ents();
		init_gen();
		init_level();
		init_player();
	}

	_obj_pal_idx = allocate_obj_pal_idx(spriteSharedPalLen);
	GRIT_CPY(pal_obj_mem + _obj_pal_idx, spriteSharedPal);

	// Tiles
	int pause_tile_offset = 0;
	switch (_data->mode)
	{
	case MG_MODE_CITY:
		_data->far_tiles_idx = allocate_bg_tile_idx(TILES_COUNT(backgroundCityTilesLen));
		GRIT_CPY(&tile_mem[MG_SHARED_CB][_data->far_tiles_idx], backgroundCityTiles);

		_data->fog_tiles_idx = allocate_bg_tile_idx(TILES_COUNT(fogTilesLen));
		GRIT_CPY(&tile_mem[MG_SHARED_CB][_data->fog_tiles_idx], fogTiles);

		pause_tile_offset = allocate_bg_tile_idx(TILES_COUNT(mgPauseCityTilesLen));
		GRIT_CPY(&tile_mem[MG_SHARED_CB][pause_tile_offset], mgPauseCityTiles);
		break;
	case MG_MODE_BEACH:
		_data->far_tiles_idx = allocate_bg_tile_idx(TILES_COUNT(mgBeachTilesLen));
		GRIT_CPY(&tile_mem[MG_SHARED_CB][_data->far_tiles_idx], mgBeachTiles);

		_data->fog_tiles_idx = allocate_bg_tile_idx(TILES_COUNT(mgBeachWaterFog00TilesLen));
		GRIT_CPY(&tile_mem[MG_SHARED_CB][_data->fog_tiles_idx], mgBeachWaterFog00Tiles);

		pause_tile_offset = allocate_bg_tile_idx(TILES_COUNT(mgPauseBeachTilesLen));
		GRIT_CPY(&tile_mem[MG_SHARED_CB][pause_tile_offset], mgPauseBeachTiles);
		break;
	}

	// Maps

	load_foreground_tiles();

	// Pause screen
	GRIT_CPY(&se_mem[MG_PAUSE_SBB], mgPauseCityMap);
	for (int i = 0; i < mgPauseCityMapLen; i++)
	{
		se_mem[MG_PAUSE_SBB][i] += pause_tile_offset / 2;
	}

	// Fill Cloud
	switch (_data->mode)
	{
	case MG_MODE_CITY:
		// City BG
		GRIT_CPY(se_mem[MG_FAR_SB], backgroundCityMap);

		// Fog
		GRIT_CPY(se_mem[MG_CLOUD_SB], fogMap);
		break;
	case MG_MODE_BEACH:
		GRIT_CPY(se_mem[MG_FAR_SB], mgBeachMap);
		// Fog
		GRIT_CPY(se_mem[MG_CLOUD_SB], mgBeachWaterFog00Map);
		se_fill(se_mem[MG_PLATFROM_SB], 0);
		break;
	}

	// Can't be bothered using char blocks
	for (int i = 0; i < mgBeachWaterFog00MapLen; i++)
		se_mem[MG_CLOUD_SB][i] += _data->fog_tiles_idx / 2;

	//Set bg postions
	REG_BG0HOFS = fx2int(_data->bg_0_scroll / 6);
	REG_BG0VOFS = 0;

	REG_BG1HOFS = 0;
	REG_BG1VOFS = 0;

	REG_BG2HOFS = fx2int(_data->bg_2_scroll / 12);
	REG_BG2VOFS = 0;

	REG_BG3HOFS = 0;
	REG_BG3VOFS = 0;

	int bg_1_prio = 0;
	int bg_2_prio = 0;
	switch (_data->mode)
	{
	case MG_MODE_BEACH:
		bg_1_prio = 2;
		bg_2_prio = 1;
		break;
	case MG_MODE_CITY:
		bg_1_prio = 1;
		bg_2_prio = 2;
		break;
	}

	// Set bkg reg
	REG_BG0CNT = BG_PRIO(3) | BG_8BPP | BG_SBB(MG_FAR_SB) | BG_CBB(MG_SHARED_CB) | BG_REG_32x32;
	REG_BG1CNT = BG_PRIO(bg_1_prio) | BG_8BPP | BG_SBB(MG_PLATFROM_SB) | BG_CBB(MG_SHARED_CB) | BG_REG_64x32;
	REG_BG2CNT = BG_PRIO(bg_2_prio) | BG_8BPP | BG_SBB(MG_CLOUD_SB) | BG_CBB(MG_SHARED_CB) | BG_REG_32x32;
	REG_BG3CNT = BG_PRIO(0) | BG_8BPP | BG_SBB(MG_PAUSE_SBB) | BG_CBB(MG_SHARED_CB) | BG_REG_32x32;

	// Load palettes
	switch (_data->mode)
	{
	case MG_MODE_CITY:
		GRIT_CPY(pal_bg_mem, mainGameCitySharedPal);
		break;
	case MG_MODE_BEACH:
		GRIT_CPY(pal_bg_mem, mainGameBeachSharedPal);
		break;
	}

	REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D | DCNT_BG0 | DCNT_BG1 | DCNT_BG2;

	switch (_data->mode)
	{
	case MG_MODE_CITY:
		//Blend reg
		REG_BLDCNT = BLD_BUILD(
			BLD_BG2, // Top layers
			BLD_BG0, // Bottom layers
			1		 // Mode
		);

		// Update blend weights
		// Left EVA: Top weight max of 15 (4 bits)
		// Right EVB: Bottom wieght max of 15 (4 bits)
		REG_BLDALPHA = BLDA_BUILD(8, 6);
		REG_BLDY = BLDY_BUILD(0);
		break;
	case MG_MODE_BEACH:
		break;
	}

	set_scroll_x(_data->starting_scroll_x);

	if (_data->fresh_game)
	{
		init_score();
	}

	// Tiles!
	load_player_tiles();
	load_health_up();
	load_gun_0_tiles();
	load_number_tiles();
	load_speed_up();
	load_jump_up();
	load_shrink_token();
	load_enemy_bullets_tiles();
	load_life_display(get_player_life());
	load_speed_level_display(get_player_speed());
	load_jump_level_display(get_player_jump());

	init_score_display();

	if (_data->fresh_game)
	{
		obj_hide_multi(oam_mem, 128);
		spawn_buildings();
	}
	else
	{
		update_score_display(get_score());
	}
}

static bool check_game_over()
{
	return _player.x + int2fx(_player.w) < 0 ||
		   _player.y > 165 * FIX_SCALE ||
		   get_player_life() <= 0;
}

static void game_over()
{
	if (_data->mode == MG_MODE_CITY)
	{
		REG_DISPCNT ^= DCNT_BG2;
	}

	REG_BLDCNT = BLD_BUILD(
		BLD_BG0 | BLD_BG1 | BLD_BG2 | BLD_OBJ, // Top layers
		0,									   // Bottom layers
		3									   // Mode
	);
	int blend = 0;
	int timer = 0;
	REG_BLDY = BLDY_BUILD(blend);

	mmStop();

	mmEffectEx(&_player_death_sound);

	while (blend <= 17 || mmEffectActive(PLAYER_ACTION_SOUND_HANDLER))
	{
		VBlankIntrWait();
		mmFrame();
		key_poll();
		timer++;
		if (timer % 8 == 0)
		{
			blend++;
			REG_BLDY = BLDY_BUILD(blend);
		}
	}
#ifdef DEBUG
	write_to_log(LOG_LEVEL_DEBUG, "GAME OVER");
#endif
	go_transfer_in_t in;
	in.score = get_score();
	set_go_in(in);
	scene_set(game_over_scene);
}

static void update(void)
{
	//Starts main track after intro
	if (!mmActive())
	{
		switch (_data->mode)
		{
		case MG_MODE_CITY:
			mmStart(MOD_PD_CITY_0, MM_PLAY_LOOP);
			break;
		case MG_MODE_BEACH:
			mmStart(MOD_PD_BEACH_0, MM_PLAY_LOOP);
			break;
		}
	}

	//Pallte cycles
	switch (_data->mode)
	{
	case MG_MODE_CITY:
		break;
	case MG_MODE_BEACH:
		if (frame_count() % 25 == 0)
		{
			cycle_palate(
				pal_bg_mem,
				4, _water_cycle,
				&_data->water_pal_idx, WATER_PAL_LENGTH);
		}
		if (frame_count() % 30 == 0)
		{
			//Copy current frame into tile mem
			dma3_cpy(
				&tile_mem[MG_SHARED_CB][_data->fog_tiles_idx],
				fog_water_cycle[_data->fog_tiles_cycle_idx].tiles,
				fog_water_cycle[_data->fog_tiles_cycle_idx].tiles_length);

			//Next frame
			++_data->fog_tiles_cycle_idx;

			//Check if need to wrap cycle
			if (_data->fog_tiles_cycle_idx >= fog_water_cycle_length)
				_data->fog_tiles_cycle_idx = 0;
		}
		break;
	}

	// Pausing!
	if (_data->state == MG_S_PAUSED)
	{
		if (key_hit(KEY_START))
		{
			REG_DISPCNT ^= DCNT_BG3;
#ifdef DEBUG
			write_to_log(LOG_LEVEL_INFO, "UNPAUSE");
#endif
			_data->state = _data->old_state;
		}
		return;
	}

	// Back to title screen
	if (key_held(KEY_SELECT) && key_hit(KEY_START))
	{
		scene_set(game_win_scene);
		return;
	}

	if (key_hit(KEY_START))
	{
#ifdef DEBUG
		write_to_log(LOG_LEVEL_INFO, "PAUSING");
#endif
		_data->old_state = _data->state;
		_data->state = MG_S_PAUSED;
		REG_DISPCNT |= DCNT_BG3;
		return;
	}

#ifdef GAME_OVER_ENABLED
	if (check_game_over())
	{
		game_over();
		return;
	}
#endif

	_bg_pos_x += _scroll_x;
	_data->bg_0_scroll += _scroll_x;
	_data->bg_2_scroll += _scroll_x;

	wrap_bkg();

	REG_BG0HOFS = fx2int(_data->bg_0_scroll / 6);
	REG_BG1HOFS = fx2int(_bg_pos_x);
	switch (_data->mode)
	{
	case MG_MODE_BEACH:
		REG_BG2HOFS = fx2int(_bg_pos_x);
		break;
	case MG_MODE_CITY:
		REG_BG2HOFS = fx2int(_data->bg_2_scroll / 4);
		break;
	}

	_data->next_building_spawn -= _scroll_x;

	if (frame_count() % 60 == 0)
		add_score(fx2int(_scroll_x * 10));

	if (_data->next_building_spawn < 0)
		spawn_buildings();

	//Copy buldings into VRAM
	for (int x = 0; x < LEVEL_WIDTH; x++)
	{
		for (int y = 0; y < LEVEL_HEIGHT; y++)
		{
			if (x < 32)
				se_plot(se_mem[MG_PLATFROM_SB], x, y, at_level(x, y));
			else
				se_plot(se_mem[MG_PLATFROM_SB + 1], x - 32, y, at_level(x, y));
		}
	}

	clear_offscreen_level();

	update_player();
	update_ents();
	update_visual_ents();
	copy_ents_to_oam();

	//Player visual effect
	if (_data->splash_active > 0)
		_data->splash_active--;

	if (
		_data->mode == MG_MODE_BEACH &&
		_data->splash_active <= 0 &&
		_player.y + int2fx(_player.h) > (BEACH_Y_SPAWN + 5) * FIX_SCALE)
	{
		create_splash_effect(allocate_visual_ent(), _player.x - 16, _player.y);
		_data->splash_active = 60;
	}

	if (_player.ent_cols & TYPE_SPEICAL_ZONE_PORTAL)
	{
		sz_transfer_in_t data;
		data.max_velocity = SZ_MAX_VELOCITY;
		data.turing_speed = SZ_TURNING_SPEED;
		data.timer_start = SZ_TIMER_SEC_END + int2fx(gba_rand_range(0, 5));
		data.obs_count = SZ_OBS_MAX_COUNT - gba_rand_range(0, 10);
		data.entered_via_debug = FALSE;
		set_sz_in(data);
		scene_set(_speical_zone_intro_scene);
		return;
	}

	if (_player.ent_cols & TYPE_IDOL)
	{
		scene_set(game_win_scene);
		return;
	}

	switch (_data->state)
	{
	case MG_S_STARTING:
		if (key_hit(KEY_RIGHT))
		{
			set_scroll_x(0.25f * FIX_SCALE);
			_data->state = MG_S_SCROLLING;
		}
		break;
	case MG_S_SCROLLING:
		if (frame_count() % SCROLL_X_RATE == 0)
		{
			set_scroll_x(_scroll_x + SCROLL_X_GAIN);
		}
		break;
	case MG_S_PAUSED:
		//This should never be hit fuck
		break;
	}
}

static void hide(void)
{
	_data->starting_scroll_x = _scroll_x;
	_out_data.last_data = *_data;

	load_blank();
	dma3_fill(se_mem[MG_CLOUD_SB], 0x0, SB_SIZE);
	dma3_fill(se_mem[MG_FAR_SB], 0x0, SB_SIZE);
	dma3_fill(se_mem[MG_PLATFROM_SB], 0x0, SB_SIZE);

	free_bg_tile_idx(0, BG_TILE_ALLC_SIZE);

	free_obj_pal_idx(_obj_pal_idx, spriteSharedPalLen);

	free_foreground_tiles();
	free_score_display();
	free_player_tiles();
	free_life_display();
	free_health_up();
	free_gun_0_tiles();
	free_number_tiles();
	free_speed_up();
	free_jump_up();
	free_shrink_token();
	free_enemy_bullets_tiles();
	free_speed_level_display();
	free_jump_level_display();

	mmStop();

	OAM_CLEAR();
	M4_CLEAR();

	SBB_CLEAR(MG_PAUSE_SBB);
	SBB_CLEAR(MG_FAR_SB);
	SBB_CLEAR(MG_CLOUD_SB);
	SBB_CLEAR(MG_PLATFROM_SB);

	CBB_CLEAR(MG_SHARED_CB);

	REG_BLDCNT = 0;
}

const scene_t main_game = {
	.show = show,
	.update = update,
	.hide = hide};
