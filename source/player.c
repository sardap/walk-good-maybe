#include "player.h"

#include <tonc.h>
#include "ent.h"

static const int SPEED = (int)(2.0f * (FIX_SCALE));
static const int GROUND_Y = (int)(120.0f * (FIX_SCALE));

ent_t _player = {};
static OBJ_ATTR* _player_obj = &_obj_buffer[0];
// static OBJ_AFFINE *_obj_aff_buffer = (OBJ_AFFINE*)_obj_buffer;

void init_player() {
	oam_init(_obj_buffer, 128);

	_player.x = 20 << FIX_SHIFT;
	_player.y = 120 << FIX_SHIFT;
	_player.tid = 0;
	_player.facing = FACING_RIGHT;

	obj_set_attr(_player_obj, 
		ATTR0_SQUARE, ATTR1_SIZE_32,
		ATTR2_PALBANK(0) | _player.tid
	);
}

void update_player() {
	if(_player.facing == FACING_RIGHT && key_hit(KEY_LEFT)) {
		_player.vx = -SPEED;
		_player.facing = FACING_LEFT;
		_player_obj->attr1 ^= ATTR1_HFLIP;
	} else if(_player.facing == FACING_LEFT && key_hit(KEY_RIGHT)) {
		_player.vx = SPEED;
		_player.facing = FACING_RIGHT;
		_player_obj->attr1 ^= ATTR1_HFLIP;
	}
	
	switch (_player.move_state)
	{
	case MOVEMENT_GROUNDED:
		if(key_hit(KEY_A)) {
			_player.vy = -SPEED;
			_player.move_state = MOVEMENT_AIR;
		}
		break;
	case MOVEMENT_AIR:
		_player.vy += (int)(1.0f * (FIX_SHIFT));
		if(_player.y >= GROUND_Y) {
			_player.y = GROUND_Y;
			_player.vy = 0;
			_player.move_state = MOVEMENT_GROUNDED;
		}
		break;
	}

	_player.x += _player.vx;
	_player.y += _player.vy;

	// increment/decrement starting tile with R/L
	_player.tid += bit_tribool(key_hit(KEY_START), KI_R, KI_L);

	// toggle mapping mode
	if(key_hit(KEY_START))
		REG_DISPCNT ^= DCNT_OBJ_1D;


	// Hey look, it's one of them build macros!
	// metr->attr2 = ATTR2_BUILD(player.tid, player.pb, 0);

	obj_set_pos(_player_obj, fx2int(_player.x), fx2int(_player.y));
	oam_copy(oam_mem, _obj_buffer, 1);	// only need to update one
}
