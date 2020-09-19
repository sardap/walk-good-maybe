#include <tonc.h>

#include "../common.h"
#include "../assets/backgroundSky.h"

const int bkgsb = 30;

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
	REG_BG0CNT = BG_CBB(0) | BG_SBB(30) | BG_4BPP | BG_REG_32x32;
	REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D | DCNT_BG0;

	// Load palette
	dma3_cpy(pal_bg_mem, backgroundSkyPal, backgroundSkyPalLen);
	// Load tiles into CBB 0
	dma3_cpy(&tile_mem[0][0], backgroundSkyTiles, backgroundSkyTilesLen);

	dma3_fill(&se_mem[bkgsb], 0x0, sb_size);
	
	for(int i = 0; i < gba_rand_range(3, 5); i++) {
		int j = 0;
		while (1)
		{
			int y = gba_rand_range(0, 10);
			int x = gba_rand_range(0, 28);
			int startAddress = 32 * y + x;
			j++;
			if(valid_cloud_address(startAddress)) {
				create_cloud_here(startAddress);
				j = 6;
			}

			if(j > 5) {
				break;
			}
		}
	}

	REG_BG0HOFS = 0;
	REG_BG0VOFS = 0;

}

static void update(void) {
	if(frame_count() % 3 == 0){
		_bkg_x++;

		REG_BG0HOFS = _bkg_x;
	}
}

const scene_t main_game = {
	.show = show,
	.update = update
};
