#include "main_game.h"

#include <tonc.h>

#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

#include "title_screen.h"
#include "../ent.h"
#include "../player.h"
#include "../graphics.h"
#include "../level.h"
#include "../debug.h"
#include "../numbers.h"
#include "../gun.h"
#include "../enemy.h"
#include "../life_display.h"
#include "../gen.h"
#include "../obstacles.h"

#include "../assets/backgroundCity.h"
#include "../assets/fog.h"
#include "../assets/mainGameShared.h"
#include "../assets/buildingtileset.h"

static FIXED _next_building_spawn;
static int _building_spawn_x;
static int _bg_0_scroll;
static int _bg_2_scroll;

static int _far_building_tiles_idx;
static int _fog_tiles_idx;

static mg_states_t _state;
static mg_states_t _old_state;
static mg_mode_t _mode;

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
	//What the fuck is nfx
	int nfx = (n * TILE_WIDTH) * FIX_SCALE;
	return fx2int(wrap_x(_bg_pos_x + nfx)) / TILE_WIDTH;
}

static void wrap_bkg()
{
	_bg_pos_x = wrap_x(_bg_pos_x);
}

static void spawn_buildings()
{
	int start_x = _building_spawn_x;

	int width = 0;
	switch (_mode)
	{
	case MG_MODE_CITY:
		switch (gba_rand() % 5)
		{
		case 0:
			width = spawn_building_0(start_x);
			break;
		case 1:
			width = spawn_building_1(start_x);
			break;
		case 2:
			width = spawn_building_2(start_x);
			break;
		case 3:
			width = spawn_building_3(start_x);
			break;
		case 4:
			width = spawn_building_4(start_x);
			break;
		}
	case MG_MODE_BEACH:
		break;
	}

	width += gba_rand_range(MIN_JUMP_WIDTH_TILES, MAX_JUMP_WIDTH_TILES);

	_building_spawn_x = level_wrap_x(start_x + width);

	_next_building_spawn = (int)((width * 8) * (FIX_SCALE));
}

static void clear_offscreen_level()
{
	int x = level_wrap_x((fx2int(_bg_pos_x) / TILE_WIDTH) - 3);
	set_level_col(x, 0, 0);
}

static void load_foreground_tiles()
{
	switch (_mode)
	{
	case MG_MODE_CITY:
		load_lava_0(MG_SHARED_CB);
		load_building_0(MG_SHARED_CB);
		load_building_1(MG_SHARED_CB);
		load_building_2(MG_SHARED_CB);
		load_building_3(MG_SHARED_CB);
		load_building_4(MG_SHARED_CB);
		break;
	case MG_MODE_BEACH:
		break;
	}
}

static void unload_foreground_tiles()
{
	switch (_mode)
	{
	case MG_MODE_CITY:
		unload_lava_0();
		unload_building_0();
		unload_building_1();
		unload_building_2();
		unload_building_3();
		unload_building_4();
		break;
	case MG_MODE_BEACH:
		break;
	}
}

static void show(void)
{
	_mode = MG_MODE_CITY;

	// Load palette
	dma3_cpy(pal_bg_mem, mainGameSharedPal, mainGameSharedPalLen);

	_bg_0_scroll = int2fx(gba_rand());
	_bg_2_scroll = int2fx(gba_rand());

	//Bad but cbf
	allocate_bg_tile_idx(528);

	_far_building_tiles_idx = 0;
	dma3_cpy(&tile_mem[MG_SHARED_CB][_far_building_tiles_idx], backgroundCityTiles, backgroundCityTilesLen);

	_fog_tiles_idx = backgroundCityTilesLen / 32;
	dma3_cpy(&tile_mem[MG_SHARED_CB][_fog_tiles_idx], fogTiles, fogTilesLen);

	load_foreground_tiles();

	load_life_display();

	//City BG
	dma3_cpy(se_mem[MG_CITY_SB], backgroundCityMap, backgroundCityMapLen);

	//Fill Cloud
	dma3_cpy(se_mem[MG_CLOUD_SB], fogMap, fogMapLen);
	//TODO: stop this double iteration bullshit
	for (int i = 0; i < fogMapLen; i++)
	{
		se_mem[MG_CLOUD_SB][i] += _fog_tiles_idx / 2;
	}

	irq_init(NULL);
	irq_add(II_VBLANK, mmVBlank);

	//Set bg postions
	REG_BG0HOFS = fx2int(_bg_0_scroll / 6);
	REG_BG0VOFS = 0;

	REG_BG1HOFS = 0;
	REG_BG1VOFS = 0;

	REG_BG2HOFS = fx2int(_bg_2_scroll / 12);
	REG_BG2VOFS = 0;

	// Set bkg reg
	REG_BG0CNT = BG_PRIO(3) | BG_8BPP | BG_SBB(MG_CITY_SB) | BG_CBB(MG_SHARED_CB) | BG_REG_32x32;
	REG_BG1CNT = BG_PRIO(1) | BG_8BPP | BG_SBB(MG_BUILDING_SB) | BG_CBB(MG_SHARED_CB) | BG_REG_64x32;
	REG_BG2CNT = BG_PRIO(2) | BG_8BPP | BG_SBB(MG_CLOUD_SB) | BG_CBB(MG_SHARED_CB) | BG_REG_32x32;
	REG_BG3CNT = BG_PRIO(0) | BG_8BPP | BG_SBB(MG_TEXT_SB) | BG_CBB(MG_SHARED_CB) | BG_REG_32x32;

	REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_BG3;

	//Blend reg
	REG_BLDCNT = BLD_BUILD(
		BLD_BG2, // Top layers
		BLD_BG0, // Bottom layers
		1		 // Mode
	);

	// Update blend weights
	//Left EVA: Top weight max of 15 (4 bits)
	//Right EVB: Bottom wieght max of 15 (4 bits)
	// REG_BLDALPHA = BLDA_BUILD(3, 5);
	REG_BLDALPHA = BLDA_BUILD(8, 6);
	REG_BLDY = BLDY_BUILD(0);

	_next_building_spawn = 0;
	_scroll_x = 0;
	_building_spawn_x = 0;
	_state = MG_S_STARTING;

	init_player();
	_player.move_state = MOVEMENT_AIR;
	load_gun_0_tiles();

	//These should be moved into level speifc stuff
	load_enemy_toast();
	load_number_tiles();
	load_speed_up();
	init_score();

	while (_building_spawn_x < LEVEL_WIDTH / 2 + LEVEL_WIDTH / 5)
	{
		spawn_buildings();
	}
}

static bool check_game_over()
{
#ifdef DEBUG
	return false;
#elif
	return fx2int(_player.x) < 0;
#endif
}

static void update(void)
{
	//Starts main track after intro
	if (!mmActive())
	{
		mmStart(MOD_PD_CITY_0, MM_PLAY_LOOP);
	}

	// Pausing!
	if (_state == MG_S_PAUSED)
	{
		if (key_hit(KEY_START))
		{
			write_to_log(LOG_LEVEL_INFO, "UNPAUSE");
			_state = _old_state;
		}
		return;
	}

	//Back to title screen
	if (key_held(KEY_START) && key_held(KEY_SELECT))
	{
		scene_set(title_screen);
		return;
	}

	if (key_hit(KEY_START))
	{
		write_to_log(LOG_LEVEL_INFO, "PAUSING");
		_old_state = _state;
		_state = MG_S_PAUSED;
		return;
	}

	if (check_game_over())
	{
		for (int i = 0; i < SB_SIZE; i++)
		{
			se_mem[MG_CITY_SB][i] = 0x0;
		}
		scene_set(title_screen);
		return;
	}

	_bg_pos_x += _scroll_x;
	_bg_0_scroll += _scroll_x;
	_bg_2_scroll += _scroll_x;

	wrap_bkg();

	REG_BG0HOFS = fx2int(_bg_0_scroll / 6);
	REG_BG1HOFS = fx2int(_bg_pos_x);
	REG_BG2HOFS = fx2int(_bg_2_scroll / 4);

	_next_building_spawn -= _scroll_x;

	if (frame_count() % 60 == 0)
	{
		add_score(fx2int(_scroll_x * 10));
	}

	if (_next_building_spawn < 0)
	{
		spawn_buildings();
	}

	//Copy buldings into VRAM
	for (int x = 0; x < LEVEL_WIDTH; x++)
	{
		for (int y = 0; y < LEVEL_HEIGHT; y++)
		{
			if (x < 32)
			{
				se_plot(se_mem[MG_BUILDING_SB], x, y, at_level(x, y));
			}
			else
			{
				se_plot(se_mem[MG_BUILDING_SB + 1], x - 32, y, at_level(x, y));
			}
		}
	}

	clear_offscreen_level();

	update_player();
	update_ents();

	oam_copy(oam_mem, _obj_buffer, att_count());

	switch (_state)
	{
	case MG_S_STARTING:
		if (key_hit(KEY_RIGHT))
		{
			_scroll_x = (int)(0.25f * FIX_SCALE);
			_state = MG_S_SCROLLING;
			//Clear text layer
			REG_DISPCNT ^= DCNT_BG3;
		}
		break;
	case MG_S_SCROLLING:
		if (frame_count() % X_SCROLL_RATE == 0)
		{
			_scroll_x += X_SCROLL_GAIN;
			//This is better than checking if it's greater prior to adding
			//Because it handles the edge case where the gain will put it much
			//over the limit
			if (_scroll_x > X_SCROLL_MAX)
			{
				_scroll_x = X_SCROLL_MAX;
			}
		}
		break;
	case MG_S_PAUSED:
		//This should never be hit fuck
		break;
	}
}

static void hide(void)
{
	REG_DISPCNT = 0;
	dma3_fill(se_mem[MG_CLOUD_SB], 0x0, SB_SIZE);
	dma3_fill(se_mem[MG_CITY_SB], 0x0, SB_SIZE);
	dma3_fill(se_mem[MG_BUILDING_SB], 0x0, SB_SIZE);

	free_bg_tile_idx(0, BG_TILE_ALLC_SIZE);

	unload_foreground_tiles();
	clear_score();
	unload_gun_0_tiles();
	unload_player();
}

const scene_t main_game = {
	.show = show,
	.update = update,
	.hide = hide};
