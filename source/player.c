#include "player.h"

#include <tonc.h>
#include "common.h"
#include "ent.h"
#include "debug.h"

static const int SPEED = (int)(2.0f * (FIX_SCALE));
static const int JUMP_POWER = (int)(1.2f * (FIX_SCALE));

ent_t _player = {};
static OBJ_ATTR* _player_obj = &_obj_buffer[0];

void init_player() {
	oam_init(_obj_buffer, 128);

	_player.tid = 0;
	_player.facing = FACING_RIGHT;
	_player.w = 16;
	_player.h = 16;

	_player.x = 20 << FIX_SHIFT;
	_player.y = PLAYER_SPAWN_Y;

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

	if(fx2int(_player.x) > 220) {
		_player.vx += -SPEED;
	}

#ifdef DEBUG
	// char str[50];
	// sprintf(str, "X: %d, VX: %f", fx2int(_player.x), fx2float(_player.vx));
	// write_to_log(LOG_LEVEL_INFO, str);
#endif

	// _player.x += _player.vx;
	_player.vx += -_scroll_x;
	ent_move_x(&_player, _player.vx);
	_player.vx = 0;
	bool hit_y = ent_move_y(&_player, _player.vy);

	if(!hit_y) {
		if(_player.vy < TERMINAL_VY) {
			_player.vy += GRAVITY;
		}
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
		if(hit_y) {
			_player.move_state = MOVEMENT_GROUNDED;
		}
		break;
	}

	if(_player.y > 160 * FIX_SCALE) {
		_player.y = PLAYER_SPAWN_Y;
	}

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
