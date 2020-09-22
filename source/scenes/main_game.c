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
static const int bg_y = 32;

static const FIXED CLOUD_WIDTH = (int)((4 * 8) * (FIX_SCALE));

static const int shared_cb = 0;

static const int cloud_sb = 24; // entries
static const int background_sb = 30; // entries

static const int shared_cloud_tile_start = 72;

static FIXED _bg_pos_x;
static FIXED _bg_pos_y;
static int _next_cloud_spawn;

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

static void spawn_cloud() {
	int x = offset_x_bg(32);
	int y = gba_rand_range(0, 10);
	int sb;
	if(x >= 32) {
		sb = cloud_sb + 1;
		x -= 32;
	} else {
		sb = cloud_sb;
	}
	se_plot(se_mem[sb], x + 0, y, shared_cloud_tile_start + 1);
	se_plot(se_mem[sb], x + 1, y, shared_cloud_tile_start + 2);
	se_plot(se_mem[sb], x + 2, y, shared_cloud_tile_start + 3);
	se_plot(se_mem[sb], x + 3, y, shared_cloud_tile_start + 4);

	se_plot(se_mem[sb], x + 0, y + 1, shared_cloud_tile_start + 5);
	se_plot(se_mem[sb], x + 1, y + 1, shared_cloud_tile_start + 6);
	se_plot(se_mem[sb], x + 2, y + 1, shared_cloud_tile_start + 7);
	se_plot(se_mem[sb], x + 3, y + 1, shared_cloud_tile_start + 8);
}

static void clear_offscreen() {
	int x = offset_x_bg(-1);
	int sb;
	if(x >= 32) {
		sb = cloud_sb + 1;
		x -= 32;
	} else {
		sb = cloud_sb;
	}

	for(int y = 0; y < 32; y++) {
		se_plot(se_mem[sb], x, y, 0);
	}
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

	// Set RegX scroll to 0
	REG_BG0HOFS = 0;
	// Set RegY scroll to 0
	REG_BG0VOFS = 0;

	// Set bkg reg
	REG_BG0CNT = BG_PRIO(1) | BG_8BPP | BG_SBB(cloud_sb) | BG_CBB(shared_cb) | BG_REG_64x32;
	REG_BG2CNT = BG_PRIO(2) | BG_8BPP | BG_SBB(background_sb) | BG_CBB(shared_cb) | BG_REG_32x32;

	REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D | DCNT_BG0 | DCNT_BG2;

	_next_cloud_spawn = 0;
	_scroll_x = (int)(0.25f * FIX_SCALE);
	
	init_player();
}

static void update(void) {
	_bg_pos_x += _scroll_x;

	wrap_bkg();

	REG_BG0HOFS = fx2int(_bg_pos_x);

	_next_cloud_spawn -= _scroll_x;

	if(_next_cloud_spawn < 0) {
		spawn_cloud();
		_next_cloud_spawn = gba_rand_range(
			fx2int(CLOUD_WIDTH),
			fx2int(CLOUD_WIDTH + (int)(100 * FIX_SCALE))
		) * FIX_SCALE;
	}

	clear_offscreen();

	update_player();

	_scroll_x += 0.001f * FIX_SCALE;
}

static void hide(void) {
	REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D;
}

const scene_t main_game = {
	.show = show,
	.update = update,
	.hide = hide
};
