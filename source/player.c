#include "player.h"

#include <tonc.h>
#include "common.h"
#include "ent.h"

static const int SPEED = (int)(2.0f * (FIX_SCALE));
static const int JUMP_POWER = (int)(1.2f * (FIX_SCALE));
static const int GROUND_Y = (int)(120.0f * (FIX_SCALE));

ent_t _player = {};
static OBJ_ATTR* _player_obj = &_obj_buffer[0];

void init_player() {
	oam_init(_obj_buffer, 128);

	_player.tid = 0;
	_player.facing = FACING_RIGHT;
	_player.w = 16;
	_player.h = 16;

	_player.x = 20 << FIX_SHIFT;
	_player.y = GROUND_Y;

	obj_set_attr(_player_obj, 
		ATTR0_SQUARE, ATTR1_SIZE_16x16,
		ATTR2_PALBANK(0) | _player.tid
	);
}

void update_player() {
	if(_player.facing == FACING_RIGHT && key_hit(KEY_LEFT)) {
		_player.facing = FACING_LEFT;
		_player_obj->attr1 ^= ATTR1_HFLIP;
	} else if(_player.facing == FACING_LEFT && key_hit(KEY_RIGHT)) {
		_player.facing = FACING_RIGHT;
		_player_obj->attr1 ^= ATTR1_HFLIP;
	}

	if(key_held(KEY_LEFT)) {
		_player.vx = -SPEED;
	} else if(key_held(KEY_RIGHT)) {
		_player.vx = SPEED;
	}

	_player.vx += -_scroll_x;

	//Push player backwards in wind
	if(_player.facing == FACING_LEFT) {
		_player.vx += -_scroll_x;
	}

	if(fx2int(_player.x) > 220) {
		_player.vx += -SPEED;
	}

	switch (_player.move_state)
	{
	case MOVEMENT_GROUNDED:
		if(key_hit(KEY_A)) {
			_player.vy = -JUMP_POWER;
			_player.move_state = MOVEMENT_AIR;
		}
		break;
	case MOVEMENT_AIR:
		_player.vy += GRAVITY;
		if(_player.y >= GROUND_Y) {
			_player.y = GROUND_Y;
			_player.vy = 0;
			_player.move_state = MOVEMENT_GROUNDED;
		}
		break;
	}

	_player.x += _player.vx;
	_player.y += _player.vy;

	_player.vx = 0;

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
