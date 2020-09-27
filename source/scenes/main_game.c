#include <tonc.h>

#include "../common.h"
#include "../ent.h"
#include "../player.h"
#include "../graphics.h"
#include "../assets/backgroundSky.h"
#include "../assets/title_text.h"
#include "../assets/titleScreenShared.h"

static const int bg_x = 64;
static const FIXED bg_x_pix = (int)(bg_x * 8 * (FIX_SCALE));

static const int BUILDINGS_SPAWN_WIDTH = 24;
static const FIXED CLOUD_WIDTH = (int)((4 * 8) * (FIX_SCALE));

static const int shared_cb = 0;

static const int cloud_sb = 24; // entries
static const int building_sb = 26; // entries
static const int background_sb = 30; // entries

static const int shared_cloud_tile_start = 72;

static u16 _level[LEVEL_SIZE];
static FIXED _bg_pos_x;
static FIXED _next_cloud_spawn;
static FIXED _next_building_spawn;
static int _temp;

static FIXED wrap_x(FIXED x) {
	if(x > bg_x_pix) {
		return x - bg_x_pix;
	} else if(x < 0) {
		return x - -x;
	}

	return x;
}

static inline int get_sb(int x) {
	return 0;
}

static inline int offset_x_bg(int n) {
	int nfx = (n * 8) * FIX_SCALE;
	return fx2int(wrap_x(_bg_pos_x + nfx)) / 8;
}

static void wrap_bkg() {
	_bg_pos_x = wrap_x(_bg_pos_x);
}

static void wrap_x_sb(int *x, int *sb) {
	if(*x >= 32) {
		*sb = *sb + 1;
		*x -= 32;
	} else {
		*sb = *sb;
	}
}

static u16 at_level(int x, int y) {
#ifdef DEBUG
	if(x > LEVEL_WIDTH) {
		dma3_fill(se_mem[background_sb], x, SB_SIZE);
		return -1;
	}
	if(y > LEVEL_HEIGHT) {
		dma3_fill(se_mem[background_sb], y, SB_SIZE);
		return -1;
	}
#endif
	return _level[LEVEL_HEIGHT * x + y];
}

static void set_level_at(int x, int y, u16 val) {
#ifdef DEBUG
	if(x > LEVEL_WIDTH) {
		dma3_fill(se_mem[background_sb], x, SB_SIZE);
		return;
	}
	if(y > LEVEL_HEIGHT) {
		dma3_fill(se_mem[background_sb], y, SB_SIZE);
		return;
	}
#endif
	_level[LEVEL_HEIGHT * x + y] = val;
}

static inline void set_col(int x, int y, u16 tile) {
	for(; y < LEVEL_HEIGHT; y++) {
		set_level_at(x, y, tile);
	}
}

static int level_wrap_x(int x) {
	if(x >= LEVEL_WIDTH) {
		return x - LEVEL_WIDTH;
	}

	if(x < 0) {
		return x + LEVEL_WIDTH;
	}

	return x;
}

static int spawn_building_0(int start_x) {
	int x_base = start_x;
	int x;
	int y = BUILDING_Y_TILE_SPAWN;
	
	//LEFT SECTION
	set_level_at(x_base, y, BUILDING_0_ROOF_LEFT);
	set_col(x_base, y + 1, BUILDING_0_ROOF_LEFT);

	int width = gba_rand_range(5, 10);
	//MIDDLE SECTION
	for(int i = 1; i < width; i++) {
		x = level_wrap_x(x_base + i);
		set_level_at(x, y, BUILDING_0_MIDDLE_ROOF);
		set_col(x, y + 1, BUILDING_0_MIDDLE_BOT);
	}

	//RIGHT SECTION
	x = level_wrap_x(x_base + width);
	set_level_at(x, y, BUILDING_0_MIDDLE_ROOF);
	set_col(x, y, BUILDING_0_ROOF_RIGHT);

	return width;
}

static void spawn_cloud() {
	int x = offset_x_bg(32);
	int y = gba_rand_range(0, 10);
	int sb = cloud_sb;
	wrap_x_sb(&x, &sb);

	se_plot(se_mem[sb], x + 0, y, shared_cloud_tile_start + 1);
	se_plot(se_mem[sb], x + 1, y, shared_cloud_tile_start + 2);
	se_plot(se_mem[sb], x + 2, y, shared_cloud_tile_start + 3);
	se_plot(se_mem[sb], x + 3, y, shared_cloud_tile_start + 4);

	se_plot(se_mem[sb], x + 0, y + 1, shared_cloud_tile_start + 5);
	se_plot(se_mem[sb], x + 1, y + 1, shared_cloud_tile_start + 6);
	se_plot(se_mem[sb], x + 2, y + 1, shared_cloud_tile_start + 7);
	se_plot(se_mem[sb], x + 3, y + 1, shared_cloud_tile_start + 8);
}

static void clear_offscreen(int sb) {
	int x = offset_x_bg(-1);
	wrap_x_sb(&x, &sb);

	for(int y = 0; y < 32; y++) {
		se_plot(se_mem[sb], x, y, 0);
	}
}

static void clear_offscreen_level() {
	int x = level_wrap_x((fx2int(_bg_pos_x) / TILE_WIDTH) - 3);
	set_col(x, 0, 0);
}

static void spawn_buildings() {
	int start_x = level_wrap_x((fx2int(_bg_pos_x) / TILE_WIDTH) + 32);
	int width = 0;
	for(int i = 0; i < BUILDINGS_SPAWN_WIDTH; i += width) {
		width = spawn_building_0(start_x);
		width += gba_rand_range(1, MAX_JUMP_WIDTH_TILES);
		start_x = level_wrap_x(start_x + width);
	}
	_next_building_spawn = (int)((BUILDINGS_SPAWN_WIDTH * 8) * (FIX_SCALE));;
	se_mem[background_sb][0] = fx2int(_next_building_spawn);
}

static void show(void) {
	// Load palette
	dma3_cpy(pal_bg_mem, titleScreenSharedPal, titleScreenSharedPalLen);
	// Load tiles into shared_cb
	dma3_cpy(&tile_mem[shared_cb], titleScreenSharedTiles, titleScreenSharedTilesLen);

	//Fill cloud layer
	for(int i = 0; i < SB_SIZE; i++) {
		se_mem[background_sb][i] = shared_cloud_tile_start;
	}

	// for(int x = 0; x < LEVEL_WIDTH; x++) {
	// 	for(int y = 0; y < LEVEL_HEIGHT; y++) {
	// 		set_level_at(x, y, (u16)x);
	// 	}
	// }

	// dma3_fill(&se_mem[cloud_sb], SB_SIZE, 0x0);
	// dma3_fill(&se_mem[background_sb], SB_SIZE, 0x0);

	// Set RegX scroll to 0
	REG_BG0HOFS = 0;
	// Set RegY scroll to 0
	REG_BG0VOFS = 0;

	// Set bkg reg
	REG_BG0CNT = BG_PRIO(2) | BG_8BPP | BG_SBB(cloud_sb) | BG_CBB(shared_cb) | BG_REG_64x32;
	REG_BG1CNT = BG_PRIO(1) | BG_8BPP | BG_SBB(building_sb) | BG_CBB(shared_cb) | BG_REG_64x32;
	REG_BG2CNT = BG_PRIO(3) | BG_8BPP | BG_SBB(background_sb) | BG_CBB(shared_cb) | BG_REG_32x32;

	REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D | DCNT_BG0 | DCNT_BG1 | DCNT_BG2;

	_next_cloud_spawn = 0;
	_next_building_spawn = 0;

	_scroll_x = (int)(0.25f * FIX_SCALE);

	_temp = 29;
	
	init_player();
}

static bool check_game_over() {
#ifdef DEBUG
	return false;
#elif
	return fx2int(_player.x) < 0;
#endif
}

static void update(void) {
	if(check_game_over()) {
		for(int i = 0; i < SB_SIZE; i++) {
			se_mem[background_sb][i] = 0x0;
		}
		scene_set(title_screen);
		return;
	}

	_bg_pos_x += _scroll_x;

	wrap_bkg();

	REG_BG0HOFS = fx2int(_bg_pos_x);
	REG_BG1HOFS = fx2int(_bg_pos_x);

	_next_cloud_spawn -= _scroll_x;
	_next_building_spawn -= _scroll_x;

	if(_next_cloud_spawn < 0) {
		spawn_cloud();
		_next_cloud_spawn = gba_rand_range(
			fx2int(CLOUD_WIDTH),
			fx2int(CLOUD_WIDTH + (int)(100 * FIX_SCALE))
		) * FIX_SCALE;
	}

	if(_next_building_spawn < 0) {
		spawn_buildings();
	}

	if(key_hit(KEY_B)) {
	}

	for(int x = 0; x < LEVEL_WIDTH; x++) {
		for(int y = 0; y < LEVEL_HEIGHT; y++) {
			if(x < 32) {
				se_plot(se_mem[building_sb], x, y, at_level(x, y));
			} else {
				se_plot(se_mem[building_sb+1], x-32, y, at_level(x, y));
			}
		}
	}

	clear_offscreen(cloud_sb);
	clear_offscreen_level();

	update_player();

	_scroll_x += 0.001f * FIX_SCALE;
}

static void hide(void) {
	REG_DISPCNT = 0;
	dma3_fill(se_mem[cloud_sb], 0x0, SB_SIZE);
	dma3_fill(se_mem[cloud_sb+1], 0x0, SB_SIZE);
}

const scene_t main_game = {
	.show = show,
	.update = update,
	.hide = hide
};
