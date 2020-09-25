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
// static const int bg_y = 32;

static const FIXED CLOUD_WIDTH = (int)((4 * 8) * (FIX_SCALE));

static const int shared_cb = 0;

static const int cloud_sb = 24; // entries
static const int building_sb = 26; // entries
static const int background_sb = 30; // entries

static const int shared_cloud_tile_start = 72;

static u16 _level[LEVEL_SIZE];
// static int _level_start;
static FIXED _bg_pos_x;
// static FIXED _bg_pos_y;
static int _next_cloud_spawn;
static int _test_x;

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

static inline void spawn_col(int sb, int x, int y, int tile) {
	for(; y < 32; y++) {
		se_plot(se_mem[sb], x, y, tile);
	}
}

static u16 at_level(int x, int y) {
	return _level[LEVEL_HEIGHT * x + y];
}

static void set_level_at(int x, int y, u16 val) {
	_level[LEVEL_HEIGHT * x + y] = val;
}

static void spawn_building_0() {
	int x_base = (_bg_pos_x >> FIX_SHIFT) / 8 + 32;
	if(x_base > 64) {
		x_base = x_base - 64;
	}
	int x;
	int y = BUILDING_Y_TILE_SPAWN;
	int sb = building_sb;
	
	se_mem[background_sb][0] = sizeof(SCR_ENTRY);

	// wrap_x_sb(&x_base, &sb);

	int width = gba_rand_range(5, 10);

	// Update level info
	 

	//LEFT SECTION
	// se_plot(se_mem[sb], x_base, y, BUILDING_0_ROOF_LEFT);
	set_level_at(y, x_base, BUILDING_0_ROOF_LEFT);
	// _level[LEVEL_WIDTH * 0 + x_base] = BUILDING_0_ROOF_LEFT;
	// spawn_col(sb, x_base, y + 1, BUILDING_0_ROOF_LEFT);

	return;
	//MIDDLE SECTION
	for(int i = 1; i < width; i++) {
		x = x_base + i;
		wrap_x_sb(&x, &sb);
		// se_plot(se_mem[sb], x, y, BUILDING_0_MIDDLE_ROOF);
		_level[LEVEL_WIDTH * y + x] = BUILDING_0_MIDDLE_ROOF;
		// spawn_col(sb, x, y + 1, BUILDING_0_MIDDLE_BOT);
	}

	//RIGHT SECTION
	wrap_x_sb(&x, &sb);
	// se_plot(se_mem[sb], x, y, BUILDING_0_ROOF_RIGHT);
	_level[LEVEL_WIDTH * y + x] = BUILDING_0_MIDDLE_ROOF;
	// spawn_col(sb, x, y + 1, BUILDING_0_ROOF_RIGHT);
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
	int x = fx2int(_bg_pos_x) / 8 - 1;
	if(x < 0){
		return;
	}
	for(int y = 0; y < LEVEL_HEIGHT; y++) {
		set_level_at(x, y, 0);
	}
}

static void spawn_buildings() {
	// int x_start = offset_x_bg(31);
	// int x;
	// int y = BUILDING_Y_TILE_SPAWN;
	// int sb = cloud_sb;

	// for(int i = 0; i < MAX_JUMP_WIDTH_TILES; i++) {
	// 	x = x_start + i;
	// 	wrap_x_sb(&x, &sb);
	// }
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

	for(int x = 0; x < LEVEL_WIDTH; x++) {
		for(int y = 0; y < LEVEL_HEIGHT; y++) {
			set_level_at(x, y, (u16)x);
		}
	}

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
	_scroll_x = (int)(0.25f * FIX_SCALE);
	_test_x = 0;
	
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
	// _level_start = fx2int(_bg_pos_x) / 8;

	wrap_bkg();

	REG_BG0HOFS = fx2int(_bg_pos_x);
	REG_BG1HOFS = fx2int(_bg_pos_x);

	_next_cloud_spawn -= _scroll_x;

	if(_next_cloud_spawn < 0) {
		spawn_cloud();
		_next_cloud_spawn = gba_rand_range(
			fx2int(CLOUD_WIDTH),
			fx2int(CLOUD_WIDTH + (int)(100 * FIX_SCALE))
		) * FIX_SCALE;
	}

	if(key_hit(KEY_B)) {
		_test_x++;
		// spawn_building_0();
	}

	for(int x = 0; x < LEVEL_WIDTH; x++) {
		for(int y = 0; y < LEVEL_HEIGHT; y++) {
			if(x > 32) {
				se_plot(se_mem[building_sb+1], x-32, y, at_level(x, y));
			} else {
				se_plot(se_mem[building_sb], x, y, at_level(x, y));
			}
		}
	}

	clear_offscreen(cloud_sb);
	clear_offscreen_level();

	spawn_buildings();

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
