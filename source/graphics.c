#include "graphics.h"

#include <tonc.h>
#include <string.h>

#include "common.h"

void init_graphics() {
}

static inline int calc_address(int width, int x, int y) {
	return  width * y + x;
}

bool valid_cloud_address(int tile_offset, int sb, int width, int x, int y) {
	int address = calc_address(width, x, y);

	for(int i = 0; i < 4; i++) {
		if(se_mem[sb][address+i] != tile_offset){
			return false;
		}
	}

	for(int i = 0; i < 4; i++) {
		if(se_mem[sb][address+32+i] != tile_offset){
			return false;
		}
	}

	return true;
}


void create_cloud(int tile_offset, int sb, int width, int x, int y) {
	int address = calc_address(width, x, y);

	se_mem[sb][address+0] 	= tile_offset + 0x1;
	se_mem[sb][address+1] 	= tile_offset + 0x2;
	se_mem[sb][address+2] 	= tile_offset + 0x3;
	se_mem[sb][address+3] 	= tile_offset + 0x4;
	se_mem[sb][address+32] 	= tile_offset + 0x5;
	se_mem[sb][address+33] 	= tile_offset + 0x6;
	se_mem[sb][address+34] 	= tile_offset + 0x7;
	se_mem[sb][address+35] 	= tile_offset + 0x8;
}

void place_n_clouds(int tile_offset, int sb, int width, int n) {
	for(int i = 0; i < n; i++) {
		int j = 0;
		while (1)
		{
			int y = gba_rand_range(0, 10);
			int x = gba_rand_range(0, 28);
			j++;
			if(valid_cloud_address(tile_offset, sb, width, x, y)) {
				create_cloud(tile_offset, sb, width, x, y);
				j = 6;
			}

			if(j > 5) {
				break;
			}
		}
	}
}