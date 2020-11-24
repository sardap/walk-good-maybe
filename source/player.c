#include "player.h"

#include <tonc.h>
#include <stdlib.h>
#include "common.h"
#include "ent.h"
#include "debug.h"
#include "graphics.h"

#include "assets/whale_small.h"
#include "assets/whale_small_jump_0.h"
#include "assets/whale_small_jump_1.h"
#include "assets/spriteShared.h"
#include "assets/whale_walk_0.h"
#include "assets/whale_walk_1.h"
#include "assets/whale_walk_2.h"
#include "assets/whale_walk_3.h"
#include "assets/whale_walk_4.h"
#include "assets/whale_land_0.h"
#include "assets/whale_land_1.h"
#include "assets/whale_air_0.h"
#include "assets/whale_air_1.h"
#include "assets/whale_air_2.h"

static const FIXED SPEED = (int)(2.0f * (FIX_SCALE));

static int _player_anime_cycle;
static int _tile_start_idx;

ent_t _player = {};

void load_player_tile() {
	_tile_start_idx = allocate_tile_idx(4);
	dma3_cpy(&tile_mem[4][_tile_start_idx], whale_smallTiles, whale_smallTilesLen);
	dma3_cpy(pal_obj_mem, spriteSharedPal, spriteSharedPalLen);
}

void init_player() {
	load_player_tile();

	_player.att_idx = allocate_att(1);
	_player_anime_cycle = 0;

	_player.tid = _tile_start_idx;
	_player.facing = FACING_RIGHT;
	_player.jump_power = (int)(2.0f * (FIX_SCALE));;
	_player.w = 16;
	_player.h = 16;

	_player.x = 20 << FIX_SHIFT;
	_player.y = PLAYER_SPAWN_Y;

	obj_set_attr(&_obj_buffer[_player.att_idx], 
		ATTR0_SQUARE | ATTR0_8BPP, ATTR1_SIZE_16x16,
		ATTR2_PALBANK(0) | _player.tid
	);
}

void unload_player() {
	free_att(1, _player.att_idx);
	free_tile_idx(_tile_start_idx, 4);
}

void update_player() {
	//Handles fliping the sprite if facing the other direction
	if(_player.facing == FACING_RIGHT && key_hit(KEY_LEFT)) {
		_player.facing = FACING_LEFT;
		get_ent_att(&_player)->attr1 ^= ATTR1_HFLIP;
	} else if(_player.facing == FACING_LEFT && key_hit(KEY_RIGHT)) {
		_player.facing = FACING_RIGHT;
		get_ent_att(&_player)->attr1 ^= ATTR1_HFLIP;
	}

	// Player movement
	if(key_held(KEY_LEFT)) {
		_player.vx = -SPEED;
	} else if(key_held(KEY_RIGHT)) {
		_player.vx = SPEED;
	}

	// Stops player from going offscreen to the right
	if(fx2int(_player.x) > GBA_WIDTH - 20) {
		_player.vx += -SPEED;
	}

	switch (_player.move_state)
	{
	case MOVEMENT_AIR:
		_player.vx /= 2;
		break;
	case MOVEMENT_JUMPING:
	case MOVEMENT_LANDED:
		_player.vx = 0;
		break;
	default:
		break;
	}

	_player.vx += -_scroll_x;
	ent_move_x(&_player, _player.vx);
	bool hit_y = ent_move_y(&_player, _player.vy);

	// Applies gravity
	if(!hit_y) {
		if(_player.vy < TERMINAL_VY) {
			_player.vy += GRAVITY;
		}
	}

	switch (_player.move_state)
	{
	case MOVEMENT_GROUNDED:
		if(abs(_player.vx) > _scroll_x) {
			const int walk_cycle_count = 25;
			if(_player_anime_cycle <= 0) {
				_player_anime_cycle = walk_cycle_count;
			}

			if(_player_anime_cycle == walk_cycle_count) {
				dma3_cpy(&tile_mem[4][_tile_start_idx], whale_walk_0Tiles, whale_walk_0TilesLen);
			} else if(_player_anime_cycle == 20) {
				dma3_cpy(&tile_mem[4][_tile_start_idx], whale_walk_1Tiles, whale_walk_1TilesLen);
			} else if(_player_anime_cycle == 15) {
				dma3_cpy(&tile_mem[4][_tile_start_idx], whale_walk_2Tiles, whale_walk_2TilesLen);
			} else if(_player_anime_cycle == 10) {
				dma3_cpy(&tile_mem[4][_tile_start_idx], whale_walk_3Tiles, whale_walk_3TilesLen);
			} else if(_player_anime_cycle == 5) {
				dma3_cpy(&tile_mem[4][_tile_start_idx], whale_walk_4Tiles, whale_walk_4TilesLen);
			} else if(_player_anime_cycle <= 0) {
				dma3_cpy(&tile_mem[4][_tile_start_idx], whale_smallTiles, whale_smallTilesLen);
				_player_anime_cycle = walk_cycle_count;
			}

			_player_anime_cycle--;
		} else {
			dma3_cpy(&tile_mem[4][_tile_start_idx], whale_smallTiles, whale_smallTilesLen);
		}
		if(key_hit(KEY_A)) {
			_player_anime_cycle = PLAYER_JUMP_TIME;
			_player.move_state = MOVEMENT_JUMPING;
		}
		break;
	
	case MOVEMENT_JUMPING:
		if(_player_anime_cycle == PLAYER_JUMP_TIME) {
			dma3_cpy(&tile_mem[4][_tile_start_idx], whale_small_jump_0Tiles, whale_small_jump_0TilesLen);
		} else if(_player_anime_cycle == PLAYER_JUMP_TIME / 2) {
			dma3_cpy(&tile_mem[4][_tile_start_idx], whale_small_jump_1Tiles, whale_small_jump_1TilesLen);
		} else if(_player_anime_cycle <= 0) {
			_player.vy = -_player.jump_power;
			_player_anime_cycle = PLAYER_AIR_CYCLE;
			_player.move_state = MOVEMENT_AIR;
			dma3_cpy(&tile_mem[4][_tile_start_idx], whale_smallTiles, whale_smallTilesLen);
		}
		_player_anime_cycle-- ;
		break;

	case MOVEMENT_AIR:
		if(hit_y) {
			_player.move_state = MOVEMENT_LANDED;
			_player_anime_cycle = PLAYER_LAND_TIME;
		}
		
		if(_player_anime_cycle == PLAYER_AIR_CYCLE) {
			dma3_cpy(&tile_mem[4][_tile_start_idx], whale_air_0Tiles, whale_air_0TilesLen);
		} else if (_player_anime_cycle == PLAYER_AIR_CYCLE / 3 * 2) {
			dma3_cpy(&tile_mem[4][_tile_start_idx], whale_air_1Tiles, whale_air_1TilesLen);
		} else if (_player_anime_cycle <= PLAYER_AIR_CYCLE / 3) {
			dma3_cpy(&tile_mem[4][_tile_start_idx], whale_air_2Tiles, whale_air_2TilesLen);
		}
		
		_player_anime_cycle--;
		if(_player_anime_cycle <= 0) {
			_player_anime_cycle = PLAYER_AIR_CYCLE;
		}
		break;

	case MOVEMENT_LANDED:
		_player.vx = 0;
		if(_player_anime_cycle == PLAYER_LAND_TIME) {
			dma3_cpy(&tile_mem[4][_tile_start_idx], whale_land_0Tiles, whale_land_0TilesLen);
		} else if (_player_anime_cycle == PLAYER_LAND_TIME / 2) {
			dma3_cpy(&tile_mem[4][_tile_start_idx], whale_land_1Tiles, whale_land_1TilesLen);
		} else if (_player_anime_cycle <= 0 ) {
			dma3_cpy(&tile_mem[4][_tile_start_idx], whale_smallTiles, whale_smallTilesLen);
			_player.move_state = MOVEMENT_GROUNDED;
		}
		_player_anime_cycle--;
		break;
	}

	_player.vx = 0;
	//if the player y wraps everything just fucks up
	if(_player.y > 160 * FIX_SCALE) {
		_player.y = PLAYER_SPAWN_Y;
	}

	// increment/decrement starting tile with R/L
	_player.tid += bit_tribool(key_hit(KEY_START), KI_R, KI_L);


	// Hey look, it's one of them build macros!
	obj_set_pos(get_ent_att(&_player), fx2int(_player.x), fx2int(_player.y));
}
