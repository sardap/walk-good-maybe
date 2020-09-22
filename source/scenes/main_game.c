#include <tonc.h>

#include "../common.h"
#include "../ent.h"
#include "../player.h"
#include "../graphics.h"
#include "../assets/backgroundSky.h"
#include "../assets/title_text.h"
#include "../assets/titleScreenShared.h"

static const int bg_x = 64;
static const int bg_x_pix = bg_x * 8;
static const int bg_y = 32;

static const int shared_cb = 0;

static const int cloud_sb = 24; // entries
static const int background_sb = 30; // entries

static const int shared_cloud_tile_start = 72;

static int _bg_pos_x;
static int _bg_pos_y;

static int wrap_x(int x) {
	if(x > bg_x_pix) {
		return x - bg_x_pix;
	} else if(x < 0) {
		return x - -x;
	}

	return x;
}

static inline int get_address(int x, int y) {
	return 0;
}

static inline int get_sb(int x) {
	return 0;
}

static void wrap_bkg() {
	_bg_pos_x = wrap_x(_bg_pos_x);
}

static void spawn_cloud() {
	// int start_x = (_bg_pos_x / 8) + 32 + 4;
	// if(start_x > bg_x) {
	// 	start_x = bg_x - start_x;
	// }
	// int start_y = gba_rand_range(0, 10);
	// int start_address;
	// if(start_x <= 32) {
	// 	start_address = 32 * start_x + start_y;
	// } else {
	// 	start_address = (32 * 32) + (32 * start_y + (start_x - 32));
	// }

	// // se_mem[background_sb][32 * 32 + (32 * 1 + 0)] = shared_cloud_tile_start + 1;
	// se_mem[background_sb][start_address] = shared_cloud_tile_start + 1;
	// se_mem[background_sb][start_address + 1] = shared_cloud_tile_start + 2;
	// se_mem[background_sb][start_address + 2] = shared_cloud_tile_start + 3;
	// se_mem[background_sb][start_address + 3] = shared_cloud_tile_start + 4;
	// se_mem[background_sb][start_address + 32] = shared_cloud_tile_start + 5;
	// se_mem[background_sb][start_address + 32 + 1] = shared_cloud_tile_start + 6;
	// se_mem[background_sb][start_address + 32 + 2] = shared_cloud_tile_start + 7;
	// se_mem[background_sb][start_address + 32 + 3] = shared_cloud_tile_start + 8;

}

static void show(void) {
	// Load palette
	dma3_cpy(pal_bg_mem, titleScreenSharedPal, titleScreenSharedPalLen);
	// Load tiles into shared_cb
	dma3_cpy(&tile_mem[shared_cb], titleScreenSharedTiles, titleScreenSharedTilesLen);

	//Fill cloud layer
	for(int i = 0; i < sb_size; i++) {
		se_mem[background_sb][i] = shared_cloud_tile_start;
	}

	spawn_cloud();

	// Set RegX scroll to 0
	REG_BG0HOFS = 0;
	// Set RegY scroll to 0
	REG_BG0VOFS = 0;

	// Set bkg reg
	REG_BG0CNT = BG_PRIO(1) | BG_8BPP | BG_SBB(cloud_sb) | BG_CBB(shared_cb) | BG_REG_64x32;
	REG_BG2CNT = BG_PRIO(2) | BG_8BPP | BG_SBB(background_sb) | BG_CBB(shared_cb) | BG_REG_32x32;

	REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D | DCNT_BG0 | DCNT_BG2;
	
	init_player();
}

static void update(void) {
	if(key_held(KEY_RIGHT) && frame_count() % 1 == 0){
		_bg_pos_x++;
	}

	if(key_held(KEY_LEFT) && frame_count() % 1 == 0){
		_bg_pos_x--;
	}

	wrap_bkg();

	REG_BG0HOFS = _bg_pos_x;

	if(key_hit(KEY_B)) {
		int x = wrap_x(_bg_pos_x + 32 * 8) / 8;
		int y = 1;
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
