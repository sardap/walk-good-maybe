#include <tonc.h>

#include "../common.h"
#include "../ent.h"
#include "../player.h"
#include "../assets/backgroundSky.h"
#include "../assets/title_text.h"
#include "../assets/titleScreenShared.h"

static const int shared_cb = 0;

static const int background_sb = 30; // entries

static u16 _bkg_x;

static void show(void) {
	// Load palette
	dma3_cpy(pal_bg_mem, titleScreenSharedPal, titleScreenSharedPalLen);
	// Load tiles into shared_cb
	dma3_cpy(&tile_mem[shared_cb], titleScreenSharedTiles, titleScreenSharedTilesLen);

	//Fill cloud layer
	for(int i = 0; i < sb_size; i++) {
		se_mem[background_sb][i] = 72;
	}

	// Set RegX scroll to 0
	REG_BG0HOFS = 0;
	// Set RegY scroll to 0
	REG_BG0VOFS = 0;

	// Set bkg reg
	REG_BG0CNT = BG_PRIO(2) | BG_8BPP | BG_SBB(background_sb) | BG_CBB(shared_cb) | BG_REG_32x32;

	REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D | DCNT_BG0;
	
	init_player();
}

static void update(void) {
	if(frame_count() % 3 == 0){
		_bkg_x++;

		REG_BG0HOFS = _bkg_x;
	}
	update_player();
}

static void hide(void) {
	REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D;
}

const scene_t main_game = {
	.show = show,
	.update = update,
	.hide = hide
};
