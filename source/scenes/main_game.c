#include "main_game.h"

#include <tonc.h>

#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

#include "../common.h"
#include "../ent.h"
#include "../player.h"
#include "../graphics.h"
#include "../level.h"
#include "../debug.h"
#include "../numbers.h"
#include "../gun.h"
#include "../enemy.h"

#include "../assets/title_text.h"
#include "../assets/backgroundCity.h"
#include "../assets/fog.h"
#include "../assets/mainGameShared.h"
#include "../assets/buildingtileset.h"

static FIXED _next_cloud_spawn;
static FIXED _next_building_spawn;
static int _building_spawn_x;
static int _tmp;
static int _bg_0_scroll;
static int _bg_2_scroll;

static int _far_building_tiles_idx;
static int _foreground_build_tile_idx;
static int _fog_tiles_idx;

static mg_states_t _state;
static mg_states_t _old_state;

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
	int nfx = (n * 8) * FIX_SCALE;
	return fx2int(wrap_x(_bg_pos_x + nfx)) / 8;
}

static void wrap_bkg()
{
	_bg_pos_x = wrap_x(_bg_pos_x);
}

static void wrap_x_sb(int *x, int *sb)
{
	if (*x >= 32)
	{
		*sb = *sb + 1;
		*x -= 32;
	}
	else
	{
		*sb = *sb;
	}
}

static int spawn_building_0(int start_x)
{
	int x_base = start_x;
	int x;
	int y = gba_rand_range(BUILDING_Y_TILE_SPAWN - 3, BUILDING_Y_TILE_SPAWN);

	//LEFT SECTION
	set_level_at(x_base, y, BUILDING_0_LEFT_ROOF + get_buildings_tile_offset());
	set_level_col(x_base, y + 1, BUILDING_0_LEFT_ROOF + get_buildings_tile_offset());

	int width = gba_rand_range(5, 10);
	//MIDDLE SECTION
	for (int i = 1; i < width; i++)
	{
		x = level_wrap_x(x_base + i);
		set_level_at(x, y, BUILDING_0_MIDDLE_ROOF + get_buildings_tile_offset());
		set_level_col(x, y + 1, BUILDING_0_MIDDLE_BOT + get_buildings_tile_offset());
	}

	//RIGHT SECTION
	x = level_wrap_x(x_base + width);
	set_level_at(x, y, BUILDING_0_MIDDLE_ROOF + get_buildings_tile_offset());
	set_level_col(x, y, BUILDING_0_RIGHT_ROOF + get_buildings_tile_offset());

	return width;
}

static int spawn_building_1(int start_x)
{
	int x_base = start_x;
	int x;
	int y = BUILDING_Y_TILE_SPAWN;

	//LEFT SECTION
	set_level_at(x_base, y, BUILDING_1_LEFT_ROOF + get_buildings_tile_offset());
	set_level_col(x_base, y + 1, BUILDING_1_LEFT_BOT + get_buildings_tile_offset());

	int width = gba_rand_range(3, 7);
	//MIDDLE SECTION
	for (int i = 1; i < width; i++)
	{
		x = level_wrap_x(x_base + i);
		set_level_at(x, y, BUILDING_1_MIDDLE_ROOF + get_buildings_tile_offset());
		set_level_col(x, y + 1, BUILDING_1_MIDDLE_BOT + get_buildings_tile_offset());
	}

	//RIGHT SECTION
	x = level_wrap_x(x_base + width);
	set_level_at(x, y, BUILDING_1_RIGHT_ROOF + get_buildings_tile_offset());
	set_level_col(x, y + 1, BUILDING_1_RIGHT_BOT + get_buildings_tile_offset());

	return width;
}

static void spawn_buildings()
{
	int start_x = _building_spawn_x;

	int width;
	if (gba_rand() % 2 == 0)
	{
		width = spawn_building_1(start_x);
	}
	else
	{
		width = spawn_building_0(start_x);
	}

	width += gba_rand_range(MIN_JUMP_WIDTH_TILES, MAX_JUMP_WIDTH_TILES);

	_building_spawn_x = level_wrap_x(start_x + width);

	_next_building_spawn = (int)((width * 8) * (FIX_SCALE));

	int count = gba_rand_range(0, 3);
	for (int i = 0; i < count; i++)
	{
		// create_toast_enemy(&_ents, int2fx(120), int2fx(30));
	}
}

static void spawn_cloud()
{
	int x = offset_x_bg(32);
	int y = gba_rand_range(0, 10);
	int sb = MG_CLOUD_SB;
	wrap_x_sb(&x, &sb);

	se_plot(se_mem[sb], x + 0, y, SKY_OFFSET + 1);
	se_plot(se_mem[sb], x + 1, y, SKY_OFFSET + 2);
	se_plot(se_mem[sb], x + 2, y, SKY_OFFSET + 3);
	se_plot(se_mem[sb], x + 3, y, SKY_OFFSET + 4);

	se_plot(se_mem[sb], x + 0, y + 1, SKY_OFFSET + 5);
	se_plot(se_mem[sb], x + 1, y + 1, SKY_OFFSET + 6);
	se_plot(se_mem[sb], x + 2, y + 1, SKY_OFFSET + 7);
	se_plot(se_mem[sb], x + 3, y + 1, SKY_OFFSET + 8);
}

static void clear_offscreen(int sb)
{
	int x = offset_x_bg(-1);
	wrap_x_sb(&x, &sb);

	for (int y = 0; y < 32; y++)
	{
		se_plot(se_mem[sb], x, y, 0);
	}
}

static void clear_offscreen_level()
{
	int x = level_wrap_x((fx2int(_bg_pos_x) / TILE_WIDTH) - 3);
	set_level_col(x, 0, 0);
}

static void show(void)
{
	// Load palette
	dma3_cpy(pal_bg_mem, mainGameSharedPal, mainGameSharedPalLen);

	_bg_0_scroll = int2fx(gba_rand());
	_bg_2_scroll = int2fx(gba_rand());
	char str[50];

	_far_building_tiles_idx = 0;
	dma3_cpy(&tile_mem[MG_SHARED_CB][_far_building_tiles_idx], backgroundCityTiles, backgroundCityTilesLen);

	_fog_tiles_idx = backgroundCityTilesLen / 32;
	dma3_cpy(&tile_mem[MG_SHARED_CB][_fog_tiles_idx], fogTiles, fogTilesLen);

	_foreground_build_tile_idx = _fog_tiles_idx + fogTilesLen / 32;
	sprintf(str, "fg:%d", _foreground_build_tile_idx / 2);
	write_to_log(LOG_LEVEL_INFO, str);
	dma3_cpy(&tile_mem[MG_SHARED_CB][_foreground_build_tile_idx], buildingtilesetTiles, buildingtilesetTilesLen);

	set_buildings_tiles_offset(_foreground_build_tile_idx / 2 + 1);

	//City BG
	dma3_cpy(se_mem[MG_CITY_SB], backgroundCityMap, backgroundCityMapLen);

	//Fill Cloud
	dma3_cpy(se_mem[MG_CLOUD_SB], fogMap, fogMapLen);
	//TODO: stop this double iteration bullshit
	for (int i = 0; i < fogMapLen; i++)
	{
		se_mem[MG_CLOUD_SB][i] += _fog_tiles_idx / 2;
	}

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

	REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D | DCNT_BG0 | DCNT_BG1 | DCNT_BG2;

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

	_next_cloud_spawn = 0;
	_next_building_spawn = 0;
	_scroll_x = 0;
	_building_spawn_x = 0;
	_state = MG_S_STARTING;

	while (_building_spawn_x < LEVEL_WIDTH / 2 + LEVEL_WIDTH / 5)
	{
		spawn_buildings();
	}

	init_player();
	_player.move_state = MOVEMENT_AIR;
	load_gun_0_tiles();

	load_number_tiles();
	init_score();
	// load_enemy_toast();

	mmSetModuleVolume(600);
	mmStart(MOD_INTRO, MM_PLAY_ONCE);
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
	if (!mmActive())
	{
		mmStart(MOD_PD_BACKGROUND_0, MM_PLAY_LOOP);
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

	_next_cloud_spawn -= _scroll_x;
	_next_building_spawn -= _scroll_x;

	if (frame_count() % 60 == 0)
	{
		add_score(fx2int(_scroll_x * 10));
	}

	if (_next_cloud_spawn < 0 && false)
	{
		spawn_cloud();
		_next_cloud_spawn = gba_rand_range(
								fx2int(CLOUD_WIDTH),
								fx2int(CLOUD_WIDTH + (int)(100 * FIX_SCALE))) *
							FIX_SCALE;
	}

	if (_next_building_spawn < 0)
	{
		spawn_buildings();
	}

	if (key_hit(KEY_B))
	{
		create_bullet(
			BULLET_TYPE_GUN_0, _player.x + int2fx(16), _player.y + int2fx(4),
			float2fx(2.5f), 0);
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

	// clear_offscreen(MG_CLOUD_SB);
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
	dma3_fill(se_mem[MG_CLOUD_SB + 1], 0x0, SB_SIZE);

	clear_score();
	unload_gun_0_tiles();
	unload_player();
}

const scene_t main_game = {
	.show = show,
	.update = update,
	.hide = hide};
