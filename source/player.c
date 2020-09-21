#include "player.h"

#include <tonc.h>
#include "ent.h"

ent_t _player = {};
OBJ_ATTR* _player_obj = &_obj_buffer[0];
// static OBJ_AFFINE *_obj_aff_buffer = (OBJ_AFFINE*)_obj_buffer;

void init_player() {
	oam_init(_obj_buffer, 128);

	_player.x = (240/2)-32;
	_player.y = 130;
	_player.tid = 0;
	_player.facing = FACING_RIGHT;

	obj_set_attr(_player_obj, 
		ATTR0_SQUARE, ATTR1_SIZE_32,
		ATTR2_PALBANK(0) | _player.tid
	);
}

void update_player() {
	obj_set_pos(_player_obj, _player.x, _player.y);
	oam_copy(oam_mem, _obj_buffer, 1);	// only need to update one
}
