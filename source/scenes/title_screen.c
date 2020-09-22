#include <tonc.h>
#include <stdio.h>

#include "../common.h"
#include "../graphics.h"
#include "../assets/backgroundSky.h"
#include "../assets/title_text.h"
#include "../assets/titleScreenShared.h"

static const int shared_cb = 0;      // tile gfx

static const int cloud_sb = 30; // entries
static const int foreground_sb = 29;

static const int shared_cloud_tile_start = 72;

static u16 _bkg_x;

static void show(void) {
	// Load palette
	dma3_cpy(pal_bg_mem, titleScreenSharedPal, titleScreenSharedPalLen);
	// Load tiles into shared_cb
	dma3_cpy(&tile_mem[shared_cb], titleScreenSharedTiles, titleScreenSharedTilesLen);
	// Copy title text to menu
	dma3_cpy(&se_mem[foreground_sb], title_textMap, title_textMapLen);

	//Fill cloud layer
	for(int i = 0; i < sb_size; i++) {
		se_mem[cloud_sb][i] = 72;
	}

	init_seed(652374291);
	place_n_clouds(
		shared_cloud_tile_start, cloud_sb, 32,
		gba_rand_range(3, 5)
	);

	// Set RegX scroll to 0
	REG_BG0HOFS = 0;
	// Set RegY scroll to 0
	REG_BG0VOFS = 0;

	// Set bkg reg
	REG_BG0CNT = BG_PRIO(2) | BG_8BPP | BG_SBB(cloud_sb) | BG_CBB(shared_cb) | BG_REG_32x32;
	REG_BG1CNT = BG_PRIO(1) | BG_8BPP | BG_SBB(foreground_sb) | BG_CBB(shared_cb) | BG_REG_32x32;

	REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D | DCNT_BG0 | DCNT_BG1;
}

static void update(void) {
	if(frame_count() % 3 == 0){
		_bkg_x++;

		REG_BG0HOFS = _bkg_x;
	}

	if(key_hit(KEY_A)) {
		init_seed(frame_count());
		scene_set(main_game);
	}
}

static void hide(void) {
	REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D | DCNT_BG0;
}

const scene_t title_screen = {
	.show = show,
	.update = update,
	.hide = hide
};
