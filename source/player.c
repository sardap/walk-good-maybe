#include "player.h"

#include <tonc.h>
#include <stdlib.h>
#include <maxmod.h>

#include "soundbank.h"
#include "soundbank_bin.h"
#include "common.h"
#include "ent.h"
#include "debug.h"
#include "graphics.h"
#include "anime.h"
#include "life_display.h"
#include "gun.h"

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

static int _player_anime_cycle;
static int _tile_start_idx;
static int _player_life;
static POINT _player_mos;
static int _invincible_frames;
static FIXED _player_speed = (int)(2.0f * (FIX_SCALE));
static int _speed_up;
static movement_state_t _move_state;
static facing_t _facing;
static FIXED _jump_power;

ent_t _player = {};

static const uint *air_anime_cycle[] = {whale_air_0Tiles, whale_air_0Tiles, whale_air_1Tiles, whale_air_2Tiles};
static const uint *walk_anime_cycle[] = {
	whale_walk_0Tiles,
	whale_walk_1Tiles,
	whale_walk_2Tiles,
	whale_walk_3Tiles,
	whale_walk_4Tiles,
	whale_smallTiles,
};

void load_player_tile()
{
	_tile_start_idx = allocate_obj_tile_idx(whale_smallTilesLen / 64);
	dma3_cpy(&tile_mem[4][_tile_start_idx], whale_smallTiles, whale_smallTilesLen);
	//This really shouldn't happen here
	dma3_cpy(pal_obj_mem, spriteSharedPal, spriteSharedPalLen);
}

void init_player()
{
	load_player_tile();

	//Reserved for player
	_player.ent_idx = 0;
	_player_anime_cycle = 0;
	_player_life = PLAYER_LIFE_START;

	_player.w = 16;
	_player.h = 16;

	_player.x = int2fx(20);
	_player.y = PLAYER_SPAWN_Y;

	_player.ent_type = TYPE_PLAYER;

	_player.att.attr0 = ATTR0_SQUARE | ATTR0_8BPP;
	_player.att.attr1 = ATTR1_SIZE_16x16;
	_player.att.attr2 = ATTR2_PALBANK(0) | ATTR2_ID(_tile_start_idx);

	_facing = FACING_RIGHT;
	_jump_power = (int)(2.0f * (FIX_SCALE));
	_move_state = MOVEMENT_AIR;
}

void unload_player()
{
	free_ent(_player.ent_idx, 1);
	free_obj_tile_idx(_tile_start_idx, 4);
}

static void apply_player_damage(int ammount)
{
	_player_life -= ammount;

	//Play sound
	mm_sound_effect damage = {
		{SFX_WHALE_DAMGE},
		(int)(1.0f * (1 << 10)),
		0,
		120,
		127,
	};
	mmEffectEx(&damage);

	//Setup moasic effect
	_player_mos.x = 32;
	_player.att.attr0 = _player.att.attr0 | ATTR0_MOSAIC;

	//Half a second
	_invincible_frames = 60;
}

static void player_shoot()
{
	//Play sound
	mm_sound_effect shoot_sound = {
		{SFX_LASER_4},
		(int)(1.0f * (1 << 10)),
		0,
		120,
		127,
	};
	mmEffectEx(&shoot_sound);

	int att_idx = allocate_ent(1);
	FIXED vx, x;
	if (_facing == FACING_RIGHT)
	{
		vx = 2.5 * FIX_SCALE;
		x = _player.x + 16 * FIX_SCALE;
	}
	else
	{
		vx = -2.5 * FIX_SCALE;
		x = _player.x + -16 * FIX_SCALE;
	}

	create_bullet(
		&_ents[att_idx], att_idx,
		BULLET_TYPE_GUN_0, x, _player.y + 4 * FIX_SCALE,
		vx, 0, _facing == FACING_LEFT);
}

void update_player()
{
	//Handles damage moasic effect
	if (frame_count() % 3 == 0 && _player_mos.x > 0)
	{
		_player_mos.x--;
		//update to be in global space
		REG_MOSAIC = MOS_BUILD(0, 0, _player_mos.x >> 3, _player_mos.y >> 3);

		if (_player_mos.x-- <= 0)
		{
			//Turn off moasic
			_player.att.attr0 ^= ATTR0_MOSAIC;
		}
	}

	//Handles fliping the sprite if facing the other direction
	if (_facing == FACING_RIGHT && key_hit(KEY_LEFT))
	{
		_facing = FACING_LEFT;
		_player.att.attr1 ^= ATTR1_HFLIP;
	}
	else if (_facing == FACING_LEFT && key_hit(KEY_RIGHT))
	{
		_facing = FACING_RIGHT;
		_player.att.attr1 ^= ATTR1_HFLIP;
	}

	// Player movement
	if (key_held(KEY_LEFT))
		_player.vx = -_player_speed;
	else if (key_held(KEY_RIGHT))
		_player.vx = _player_speed;

	//Shoot
	if (key_hit(KEY_B))
		player_shoot();

	// Stops player from going offscreen to the right
	if (fx2int(_player.x) > GBA_WIDTH - 20)
	{
		_player.vx += -_player_speed;
	}

	// Update velocity
	switch (_move_state)
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
	if (!hit_y)
	{
		if (_player.vy < TERMINAL_VY)
		{
			_player.vy += GRAVITY;
		}
	}

	//Apply all damage
	if (_invincible_frames <= 0)
	{
		//Lava damage
		if (ent_level_collision(&_player) & (LEVEL_LAVA))
		{
			_player.vy -= LAVA_BOUNCE;
			_player_anime_cycle = PLAYER_AIR_CYCLE_COUNT;
			_move_state = MOVEMENT_AIR;
			apply_player_damage(1);
		}
		//Enemy Damage
		if (_player.ent_cols & (TYPE_ENEMY))
		{
			apply_player_damage(1);
		}
	}
	else
	{
		_invincible_frames--;
	}

	//Check colsion with other
	if (_speed_up <= 0 && _player.ent_cols & (TYPE_SPEED_UP) && _scroll_x > 0)
	{
		_speed_up = 120;
		_scroll_x += 0.5f * FIX_SCALEF;
		_player_speed += 0.5f * FIX_SCALEF;
	}
	else if (_speed_up > 0)
	{
		--_speed_up;
		//Check ended and handle ended
		if (_speed_up <= 0)
			_scroll_x -= 0.5f * FIX_SCALEF;
	}

	//Handles player anime
	switch (_move_state)
	{
	case MOVEMENT_GROUNDED:
		if (abs(_player.vx) > _scroll_x)
		{
			step_anime_bad(
				walk_anime_cycle, whale_smallTilesLen, PLAYER_WALK_CYCLE_COUNT,
				&_player_anime_cycle, _tile_start_idx);
		}
		else
		{
			dma3_cpy(&tile_mem[4][_tile_start_idx], whale_smallTiles, whale_smallTilesLen);
		}
		if (key_hit(KEY_A))
		{
			_player_anime_cycle = PLAYER_JUMP_TIME;
			_move_state = MOVEMENT_JUMPING;
		}
		break;

	case MOVEMENT_JUMPING:
		if (_player_anime_cycle == PLAYER_JUMP_TIME)
		{
			dma3_cpy(&tile_mem[4][_tile_start_idx], whale_small_jump_0Tiles, whale_small_jump_0TilesLen);
		}
		else if (_player_anime_cycle == PLAYER_JUMP_TIME / 2)
		{
			dma3_cpy(&tile_mem[4][_tile_start_idx], whale_small_jump_1Tiles, whale_small_jump_1TilesLen);
		}
		else if (_player_anime_cycle <= 0)
		{
			_player.vy = -_jump_power;
			_player_anime_cycle = PLAYER_AIR_CYCLE_COUNT;
			_move_state = MOVEMENT_AIR;
			dma3_cpy(&tile_mem[4][_tile_start_idx], whale_smallTiles, whale_smallTilesLen);
		}
		_player_anime_cycle--;
		break;

	case MOVEMENT_AIR:
		if (hit_y)
		{
			_move_state = MOVEMENT_LANDED;
			_player_anime_cycle = PLAYER_LAND_TIME;
		}

		step_anime_bad(
			air_anime_cycle, whale_air_0TilesLen, PLAYER_AIR_CYCLE_COUNT,
			&_player_anime_cycle, _tile_start_idx);

		break;

	case MOVEMENT_LANDED:
		_player.vx = 0;
		if (_player_anime_cycle == PLAYER_LAND_TIME)
		{
			dma3_cpy(&tile_mem[4][_tile_start_idx], whale_land_0Tiles, whale_land_0TilesLen);
		}
		else if (_player_anime_cycle == PLAYER_LAND_TIME / 2)
		{
			dma3_cpy(&tile_mem[4][_tile_start_idx], whale_land_1Tiles, whale_land_1TilesLen);
		}
		else if (_player_anime_cycle <= 0)
		{
			dma3_cpy(&tile_mem[4][_tile_start_idx], whale_smallTiles, whale_smallTilesLen);
			_move_state = MOVEMENT_GROUNDED;
		}
		_player_anime_cycle--;
		break;
	}

	_player.vx = 0;
	//if the player y wraps everything just fucks up
	if (_player.y > 160 * FIX_SCALE)
	{
		_player.y = PLAYER_SPAWN_Y;
	}
}

int get_player_life()
{
	return _player_life;
}

int speed_up_active()
{
	return _speed_up;
}