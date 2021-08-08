#include "player.h"

#include <tonc.h>
#include <stdlib.h>
#include <maxmod.h>

#include "common.h"
#include "ent.h"
#include "debug.h"
#include "graphics.h"
#include "anime.h"
#include "gun.h"
#include "ui_display.h"
#include "sound.h"

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

static const uint *air_anime_cycle[] = {whale_air_0Tiles, whale_air_0Tiles, whale_air_1Tiles, whale_air_2Tiles};
static const uint *walk_anime_cycle[] = {
	whale_walk_0Tiles,
	whale_walk_1Tiles,
	whale_walk_2Tiles,
	whale_walk_3Tiles,
	whale_walk_4Tiles,
	whale_smallTiles,
};

static int _player_anime_cycle;
static int _tile_start_idx;
static int _player_life;
static int _invincible_frames;
static int _speed_up_active;
static movement_state_t _move_state;
static facing_t _facing;
static POINT _player_mos;
static FIXED _player_speed;
static FIXED _player_air_slowdown;
static FIXED _player_jump_power;
static FIXED _sx, _sy;
static int _shrinking;
ent_t _player = {};

void init_player()
{
	//Reserved for player
	_player.ent_idx = 0;
	_player_anime_cycle = 0;
	_player_life = PLAYER_LIFE_START;

	_player.w = 16;
	_player.h = 16;

	_player.x = int2fx(20);
	_player.y = PLAYER_SPAWN_Y;

	_player.ent_type = TYPE_PLAYER;

	_player.att.attr0 = ATTR0_SQUARE | ATTR0_8BPP | ATTR0_AFF;
	_player.att.attr1 = ATTR1_SIZE_16;
	_player.att.attr2 = ATTR2_PRIO(PLAYER_PRIO) | ATTR2_ID(_tile_start_idx);
	obj_aff_identity(&_player.aff);

	_facing = FACING_RIGHT;
	_player_jump_power = PLAYER_START_JUMP_POWER;
	_move_state = MOVEMENT_AIR;

	_player_speed = (int)(2.0f * (FIX_SCALE));
	_player_air_slowdown = PLAYER_AIR_START_SLOWDOWN;

	_sx = 1 * FIX_SCALE;
	_sy = 1 * FIX_SCALE;
}

void load_player_tiles()
{
	_tile_start_idx = allocate_obj_tile_idx(whale_smallTilesLen / 64);
	dma3_cpy(&tile_mem[4][_tile_start_idx], whale_smallTiles, whale_smallTilesLen);
	//This really shouldn't happen here
	dma3_cpy(pal_obj_mem, spriteSharedPal, spriteSharedPalLen);
}

void free_player_tiles()
{
	free_obj_tile_idx(_tile_start_idx, whale_smallTilesLen / 64);
}

FIXED get_player_jump()
{
	return fxdiv(_player_jump_power, 6 * FIX_SCALE);
}

void add_player_jump(FIXED amount)
{
	_player_jump_power = clamp(
		_player_jump_power + amount,
		PLAYER_START_JUMP_POWER, PLAYER_MAX_JUMP_POWER);

	char str[50];
	sprintf(str, "JUMP:%2.f AMT:%2.f", fx2float(_player_jump_power), fx2float(amount));
	write_to_log(LOG_LEVEL_DEBUG, str);

	update_jump_level_display(get_player_jump());
}

FIXED get_player_speed()
{
	return fxdiv(_player_air_slowdown, 6 * FIX_SCALE);
}

void add_player_speed(FIXED amount)
{
	_player_air_slowdown = clamp(
		_player_air_slowdown - amount,
		PLAYER_AIR_SLOWDOWN_MIN, PLAYER_AIR_START_SLOWDOWN);

	char str[50];
	sprintf(str, "SPEED:%2.f AMT:%2.f", fx2float(_player_air_slowdown), fx2float(amount));
	write_to_log(LOG_LEVEL_DEBUG, str);

	update_speed_level_display(get_player_speed());
}

void free_player()
{
	free_obj_tile_idx(_tile_start_idx, 4);
}

static void apply_player_damage(int ammount)
{
	_player_life -= ammount;

	//Play sound
	mmEffectEx(&_player_damage);

	//Setup moasic effect
	_player_mos.x = 32;
	_player.att.attr0 = _player.att.attr0 | ATTR0_MOSAIC;

	//Half a second
	_invincible_frames = 60;
}

static void player_shoot()
{
	//Play sound
	mmEffectEx(&_player_shoot_sound);

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
		allocate_ent(),
		BULLET_TYPE_GUN_0, x, _player.y + 4 * FIX_SCALE,
		vx, 0, _facing == FACING_LEFT);
}

void update_player()
{
	// What the fuck is this?
	// static int scale = 1;
	// if (key_held(KEY_L) || key_held(KEY_R))
	// {
	// 	if (key_held(KEY_L))
	// 		scale += 128;
	// 	if (key_held(KEY_R))
	// 		scale -= 128;

	// 	char str[50];
	// 	sprintf(str, "scale:%d", scale);
	// 	write_to_log(LOG_LEVEL_DEBUG, str);
	// }

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

	/** Handles updating the facing direction
		NOTE DO NOT USE HFLIP on Affine sprites
	 	you need to invert the x scale or y scale to flip
	*/
	if (key_hit(KEY_LEFT))
	{
		_facing = FACING_LEFT;
	}
	else if (key_hit(KEY_RIGHT))
	{
		_facing = FACING_RIGHT;
	}

	// Player movement
	if (key_held(KEY_LEFT))
	{
		_player.vx = -_player_speed;
	}
	else if (key_held(KEY_RIGHT))
	{
		_player.vx = _player_speed;
	}

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
		_player.vx = fxdiv(_player.vx, get_player_speed());
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

	//Player movmenet sound
	if (!mmEffectActive(PLAYER_WALK_SOUND_HANDLER))
	{
		//Flapping sound
		if (!hit_y)
		{
			mmEffectEx(&_player_flap_sound);
		}
		//Walking sound
		else if (_player.vx > 0 || _player.vx < -_scroll_x)
		{
			mmEffectEx(&_player_walk_sound);
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
		if (_player.ent_cols & (TYPE_ENEMY_BISCUT | TYPE_ENEMY_BISCUT_UFO | TYPE_ENEMY_BULLET))
		{
			apply_player_damage(1);
		}
	}
	else
	{
		_invincible_frames--;
	}

	//Check colsion with speed_up
	if (_speed_up_active <= 0 && _player.ent_cols & (TYPE_SPEED_UP) && _scroll_x > 0)
	{
		_speed_up_active = 120;
		_scroll_x += 0.5f * FIX_SCALEF;
		// Lower air slowdown speed
		add_player_speed(PLAYER_ADD_SPEED_STEP);
	}
	else if (_speed_up_active > 0)
	{
		--_speed_up_active;
		// Check ended and handle ended
		if (_speed_up_active <= 0)
			_scroll_x -= 0.5f * FIX_SCALEF;
	}

	// Health up
	if (_player.ent_cols & (TYPE_HEALTH_UP))
	{
		_player_life = clamp(_player_life + 1, 0, PLAYER_LIFE_START + 1);
	}

	// Jump up
	if (_player.ent_cols & (TYPE_JUMP_UP))
	{
		add_player_jump(PLAYER_ADD_JUMP_STEP);
	}

	// Shrink Token
	if (_player.ent_cols & (TYPE_SHRINK_TOKEN))
	{
		if (!_shrinking)
			mmEffectEx(&_player_shrink_sound);

		_shrinking = PLAYER_SHRINKING_TIME;
	}

	// Speical zone checking happens within the main_game becuase this is
	// A bowel of spaghtiti

	// Shirnking
	FIXED size_step = PLAYER_SHRINK_STEP;
	if (_shrinking)
	{
		size_step = -size_step;

		--_shrinking;
		if (_shrinking <= 0)
		{
			mmEffectEx(&_player_grow_sound);
			_shrinking = 0;
		}
	}

	_sx = clamp(_sx + size_step, PLAYER_SHRINK_SIZE, PLAYER_FULL_SIZE);
	_sy = clamp(_sy + size_step, PLAYER_SHRINK_SIZE, PLAYER_FULL_SIZE);
	_player.w = fx2int(fxmul(_sx, 16 * FIX_SCALE));
	_player.h = fx2int(fxmul(_sy, 16 * FIX_SCALE));

	// Handles player anime
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
			mmEffectCancel(PLAYER_WALK_SOUND_HANDLER);
			mmEffectEx(&_player_jump_sound);
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
			_player.vy = -get_player_jump();
			_player_anime_cycle = PLAYER_AIR_CYCLE_COUNT;
			_move_state = MOVEMENT_AIR;
			dma3_cpy(&tile_mem[4][_tile_start_idx], whale_smallTiles, whale_smallTilesLen);
		}
		_player_anime_cycle--;
		break;

	case MOVEMENT_AIR:
		if (hit_y)
		{
			mmEffectEx(&_player_land_sound);
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

	int sx = _facing == FACING_RIGHT ? _sx : -_sx;
	obj_aff_rotscale(
		&_player.aff,
		//Invert sacle
		((1 << 24) / sx) >> 8, ((1 << 24) / _sy) >> 8, 0);

	obj_set_pos(
		&_player.att,
		fx2int(_player.x) + (16 - fx2int(fxmul(16 * FIX_SCALE, _sx))),
		fx2int(_player.y) + (16 - fx2int(fxmul(16 * FIX_SCALE, _sy))));
}

int get_player_life()
{
	return _player_life;
}

int speed_up_active()
{
	return _speed_up_active;
}