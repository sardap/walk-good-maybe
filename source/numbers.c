#include "numbers.h"

#include <tonc.h>
#include "ent.h"
#include "graphics.h"
#include "assets/numbersfont.h"
#include "assets/spriteShared.h"

static int _tile_start_idx;
static ent_t number_test;

void load_number_tiles() {
	_tile_start_idx = allocate_tile_idx(10);
	dma3_cpy(&tile_mem[4][4], numbersfontTiles, numbersfontTilesLen);
	dma3_cpy(pal_obj_mem, spriteSharedPal, spriteSharedPalLen);

// 	number_test.att_idx = allocate_att(1);
// 	number_test.tid = _tile_start_idx;
// 	number_test.x = 0;
// 	number_test.y = 0;

// 	obj_set_attr(&_obj_buffer[_player.att_idx], 
// 		ATTR0_SQUARE, ATTR1_SIZE_8x8,
// 		ATTR2_PALBANK(0) | number_test.tid
// 	);
}

void unload_numbers() {
	free_tile_idx(_tile_start_idx, 10);
}

void test_numbers() {
	
}

