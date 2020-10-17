#include "numbers.h"

#include <tonc.h>
#include "ent.h"
#include "graphics.h"
#include "assets/numbersfont.h"
#include "assets/spriteShared.h"
#include "debug.h"
#include "common.h"

static int _tile_start_idx;
static ent_t number_test;
static int _number;

void load_number_tiles() {
	_tile_start_idx = allocate_tile_idx(10);
	dma3_cpy(&tile_mem[4][_tile_start_idx], numbersfontTiles, numbersfontTilesLen);
	dma3_cpy(pal_obj_mem, spriteSharedPal, spriteSharedPalLen);

	number_test.att_idx = allocate_att(1);
	number_test.tid = _tile_start_idx;
	number_test.x = 0;
	number_test.y = 0;

	_number = 0;
	update_number();
}

void update_number() {
	obj_set_attr(&_obj_buffer[number_test.att_idx], 
		ATTR0_SQUARE, ATTR1_SIZE_8x8,
		ATTR2_PALBANK(0) | ATTR2_PRIO(0) | ATTR2_ID(number_test.tid + _number)
	);

	obj_set_pos(get_ent_att(&number_test), fx2int(number_test.x), fx2int(number_test.y));
	oam_copy(oam_mem, _obj_buffer, number_test.att_idx + 1);	// only need to update one
}

void unload_numbers() {
	free_tile_idx(_tile_start_idx, 10);
}

void test_numbers() {
	if(frame_count() % 60 == 0) {
		if(_number + 1 > 9) {
			_number = 0;
		} else {
			_number++;
		}
		update_number();
	}
}

