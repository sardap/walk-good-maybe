#include <tonc.h>

#include "../common.h"
#include "../assets/backgroundSky.h"
#include "../assets/title_text.h"
#include "../assets/titleScreenShared.h"

static const int bkgsb = 30;

static const int shared_palbank = 0;
static const int shared_cb = 0;      // tile gfx

static const int background_sb = 30; // entries
static const int foreground_sb = 29;

static u16 _bkg_x;

static bool valid_cloud_address(int start_address) {
	for(int i = 0; i < 4; i++) {
		if(se_mem[bkgsb][start_address+i]){
			return false;
		}
	}

	for(int i = 0; i < 4; i++) {
		if(se_mem[bkgsb][start_address+32+i]){
			return false;
		}
	}

	return true;
}

static void create_cloud_here(int start_address) {
	se_mem[bkgsb][start_address+0] = 0x1;
	se_mem[bkgsb][start_address+1] = 0x2;
	se_mem[bkgsb][start_address+2] = 0x3;
	se_mem[bkgsb][start_address+3] = 0x4;
	se_mem[bkgsb][start_address+32] = 0x5;
	se_mem[bkgsb][start_address+33] = 0x6;
	se_mem[bkgsb][start_address+34] = 0x7;
	se_mem[bkgsb][start_address+35] = 0x8;

}

static void show(void) {
	// Load palette
	dma3_cpy(pal_bg_mem, titleScreenSharedPal, titleScreenSharedPalLen);
	// Load tiles into CBB 0
	dma3_cpy(&tile_mem[shared_cb], titleScreenSharedTiles, titleScreenSharedTilesLen);
	
	// for(int i = 0; i < gba_rand_range(3, 5); i++) {
	// 	int j = 0;
	// 	while (1)
	// 	{
	// 		int y = gba_rand_range(0, 10);
	// 		int x = gba_rand_range(0, 28);
	// 		int startAddress = 32 * y + x;
	// 		j++;
	// 		if(valid_cloud_address(startAddress)) {
	// 			create_cloud_here(startAddress);
	// 			j = 6;
	// 		}

	// 		if(j > 5) {
	// 			break;
	// 		}
	// 	}
	// }

	// dma3_cpy(pal_bg_mem, title_textPal, title_textPalLen);
	// dma3_fill(&se_mem[background_sb], 72, 1);
	for(int i = 0; i < sb_size; i++) {
		se_mem[background_sb][i] = 72;
	}
	dma3_cpy(&se_mem[foreground_sb], title_textMap, title_textMapLen);


	REG_BG0HOFS = 0;
	REG_BG0VOFS = 0;

	REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D | DCNT_BG0 | DCNT_BG1;

	REG_BG0CNT = BG_PRIO(2) | BG_8BPP | BG_SBB(background_sb) | BG_CBB(shared_cb) | BG_REG_32x32;
	REG_BG1CNT = BG_PRIO(1) | BG_8BPP | BG_SBB(foreground_sb) | BG_CBB(shared_cb) | BG_REG_32x32;
}

static void update(void) {
	if(frame_count() % 3 == 0){
		_bkg_x++;

		// REG_BG0HOFS = _bkg_x;
	}
}

const scene_t title_screen = {
	.show = show,
	.update = update
};
