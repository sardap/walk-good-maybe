#include "ent.h"

#include <tonc.h>

#include "common.h"
#include "level.h"
#include "debug.h"
#include "obstacles.h"
#include "gun.h"
#include "enemy.h"
#include "player.h"
#include "ui_display.h"
#include "effect.h"

OBJ_ATTR _obj_buffer[OBJ_COUNT] = {};
ent_t _ents[ENT_COUNT] = {};
visual_ent_t _visual_ents[ENT_VISUAL_COUNT] = {};

static OBJ_AFFINE _aff_buffer[ENT_COUNT] = {};
static u8 _allocated_ents[ENT_COUNT];
static u8 _allocated_visual_ents[ENT_VISUAL_COUNT];

void init_all_ents()
{
	oam_init(_obj_buffer, OBJ_COUNT);

	for (int i = 0; i < ENT_COUNT; i++)
	{
		_allocated_ents[i] = 0;
		_ents[i].ent_type = TYPE_NONE;
	}
	//Player? maybe I don't know
	_allocated_ents[0] = 1;

	for (int i = 0; i < ENT_VISUAL_COUNT; i++)
	{
		_allocated_visual_ents[i] = 0;
		_visual_ents[i].type = TYPE_VISUAL_NONE;
	}
}

static int allocate(u8 *ary, int length, int count)
{
	for (int i = 0; i < length;)
	{
		bool found = true;
		for (int j = i; j - i < count && j < length; j++)
		{
			if (ary[j] > 0)
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			for (int j = 0; j < count; j++)
			{
				ary[i + j] = 1;
			}
			return i;
		}
		i += count;
	}

#ifdef DEBUG
	char str[200];
	sprintf(str, "FAILLED TO ENT ALLOCATE s:%d l:%d", count, length);
	write_to_log(LOG_LEVEL_DEBUG, str);

	str[0] = '\0';
	for (int i = 0; i < length; i++)
	{
		char b[50];
		sprintf(b, "%d, ", ary[i]);
		strcat(str, b);
	}

	write_to_log(LOG_LEVEL_DEBUG, str);
#endif
	return 0;
}

ent_t *allocate_ent()
{
	int idx = allocate(_allocated_ents, ENT_COUNT, 1);
	_ents[idx].ent_idx = idx;
	return &_ents[idx];
}

void free_ent(ent_t *ent)
{
	_allocated_ents[ent->ent_idx] = 0;
	ent->ent_idx = 0;
	ent->ent_type = TYPE_NONE;
}

void complete_free_ent(ent_t *e)
{
	switch (e->ent_type)
	{
	case TYPE_NONE:
	case TYPE_PLAYER:
		break;
	case TYPE_BULLET:
	case TYPE_SPEED_UP:
	case TYPE_HEALTH_UP:
	case TYPE_SHRINK_TOKEN:
	case TYPE_ENEMY_BULLET:
	case TYPE_JUMP_UP:
		free_ent(e);
		break;
	case TYPE_ENEMY_BISCUT:
		free_enemy_biscut(e);
		break;
	case TYPE_ENEMY_BISCUT_UFO:
		free_enemy_ufo_bisuct(e);
		break;
	case TYPE_SPEICAL_ZONE_PORTAL:
		free_speical_zone_portal(e);
		break;
	case TYPE_IDOL:
		free_idol(e);
		break;
	}
}

visual_ent_t *allocate_visual_ent()
{
	int idx = allocate(_allocated_visual_ents, ENT_VISUAL_COUNT, 1);
	_visual_ents[idx].ent_idx = idx;
	return &_visual_ents[idx];
}

void free_visual_ent(visual_ent_t *ent)
{
	_allocated_visual_ents[ent->ent_idx] = 0;
	ent->ent_idx = 0;
	ent->type = TYPE_VISUAL_NONE;
}

void complete_free_visual_ent(visual_ent_t *v_ent)
{
	switch (v_ent->type)
	{
	case TYPE_NONE:
		break;
	case TYPE_VISUAL_LIFE:
	case TYPE_VISUAL_SPEED_LINE:
	case TYPE_VISUAL_SPEED_LEVEL:
	case TYPE_VISUAL_SCORE:
	case TYPE_VISUAL_JUMP_LEVEL:
		free_visual_ent(v_ent);
		break;
	case TYPE_VISUAL_ENEMY_BISUCT_DEATH:
		free_enemy_biscut_death(v_ent);
		break;
	case TYPE_VISUAL_ENEMY_BISUCT_UFO_DEATH:
		free_enemy_ufo_biscut_death(v_ent);
		break;
	case TYPE_VISUAL_SPLASH:
		free_splash_effect(v_ent);
		break;
	}
}

void free_all_ents()
{
	for (int i = 0; i < ENT_COUNT; i++)
	{
		ent_t *ent = &_ents[i];
		complete_free_ent(ent);
	}

	for (int i = 0; i < ENT_VISUAL_COUNT; i++)
	{
		visual_ent_t *ent = &_visual_ents[i];
		complete_free_visual_ent(ent);
	}
}

void copy_ents_to_oam()
{
	int obj_idx = 0;
	int aff_idx = 0;

	//Don't update postions here since it fucks with affine sprites dbl size
	for (int i = 0; i < ENT_COUNT; i++)
	{
		ent_t *ent = &_ents[i];

		if (ent->ent_type == TYPE_NONE)
			continue;

		OBJ_ATTR tmp = ent->att;

		//Check if sprite is affine
		if (tmp.attr0 & (ATTR0_AFF))
		{
			//Tells sprite which affine matrix to look at
			tmp.attr1 |= ATTR1_AFF_ID(aff_idx);
			//Copies the affine matrix from ent
			_aff_buffer[aff_idx] = ent->aff;
			++aff_idx;
		}

		_obj_buffer[obj_idx] = tmp;

		++obj_idx;
	}

	for (int i = 0; i < ENT_VISUAL_COUNT; i++)
	{
		visual_ent_t *ent = &_visual_ents[i];

		if (ent->type == TYPE_VISUAL_NONE)
			continue;

		_obj_buffer[obj_idx] = ent->att;
		obj_set_pos(&_obj_buffer[obj_idx], fx2int(ent->x), fx2int(ent->y));

		++obj_idx;
	}

	for (int i = obj_idx; i < ENT_COUNT + ENT_VISUAL_COUNT; i++)
	{
		obj_set_attr(&_obj_buffer[i], ATTR0_HIDE, 0, 0);
	}

	for (int i = aff_idx; i < ENT_COUNT; i++)
	{
		obj_aff_identity(&_aff_buffer[aff_idx]);
	}

	obj_copy(obj_mem, _obj_buffer, ENT_COUNT + ENT_VISUAL_COUNT);
	obj_aff_copy(obj_aff_mem, _aff_buffer, ENT_COUNT);
}

FIXED translate_x(ent_t *e)
{
	return e->x + _bg_pos_x;
}

FIXED translate_y(ent_t *e)
{
	return e->y;
}

// STOLEN from https://github.com/exelotl/goodboy-advance
bool did_hit_x(ent_t *e, FIXED dx)
{
	int flags = ent_level_collision_at(e, dx, 0);
	return flags & (LEVEL_COL_GROUND);
}

bool ent_move_x(ent_t *e, FIXED dx)
{
	if (did_hit_x(e, dx))
	{

		if (dx > 0)
		{
			while (dx > 0 && !did_hit_x(e, -(1 * FIX_SCALE)))
			{
				e->x -= 1 * FIX_SCALE;
				dx -= 1 * FIX_SCALE;
			}
		}

		return true;
	}
	e->x += e->vx;

	return false;
}

void ent_move_x_dirty(ent_t *e)
{
	e->x += e->vx;
}

// STOLEN from https://github.com/exelotl/goodboy-advance
bool did_hit_y(ent_t *e, FIXED dy)
{
	int flags = ent_level_collision_at(e, 0, dy);
	return flags & (LEVEL_COL_GROUND);
}

// STOLEN from https://github.com/exelotl/goodboy-advance
bool ent_move_y(ent_t *e, FIXED dy)
{
	if (did_hit_y(e, dy))
	{
		int sign = dy >= 0 ? 1 * FIX_SCALE : -1 * FIX_SCALE;
		while (dy != 0 && !did_hit_y(e, sign))
		{
			e->y += sign;
			dy -= sign;
		}

		return true;
	}

	e->y += e->vy;
	return false;
}

void ent_move_y_dirty(ent_t *e)
{
	e->y += e->vy;
}

void update_ents()
{
	//Add player to ent array
	_ents[0] = _player;

	for (int i = 0; i < ENT_COUNT; i++)
	{
		if (_ents[i].ent_type == TYPE_NONE)
			continue;

		//reset col
		_ents[i].ent_cols = TYPE_NONE;

		for (int j = 0; j < ENT_COUNT; j++)
		{
			if (
				//Don't check vs self
				i != j &&
				//Don't check vs unallocated ents
				_ents[j].ent_type != TYPE_NONE &&
				//Rect intersection
				_ents[i].x < _ents[j].x + int2fx(_ents[j].w) &&
				_ents[i].x + int2fx(_ents[i].w) > _ents[j].x &&
				_ents[i].y < _ents[j].y + int2fx(_ents[j].h) &&
				_ents[i].y + int2fx(_ents[i].h) > _ents[j].y)
			{
				_ents[i].ent_cols |= _ents[j].ent_type;
			}
		}
	}

	//Copy updated player ent back into player
	_player = _ents[0];

	for (int i = 0; i < ENT_COUNT; i++)
	{
		switch (_ents[i].ent_type)
		{
		case TYPE_NONE:
			break;
		case TYPE_BULLET:
			update_bullet(&_ents[i]);
			break;
		case TYPE_ENEMY_BISCUT:
			update_enemy_biscut(&_ents[i]);
			break;
		case TYPE_PLAYER:
			break;
		case TYPE_SPEED_UP:
			update_speed_up(&_ents[i]);
			break;
		case TYPE_ENEMY_BISCUT_UFO:
			update_enemy_ufo_bisuct(&_ents[i]);
			break;
		case TYPE_ENEMY_BULLET:
			update_enemy_bullet(&_ents[i]);
			break;
		case TYPE_HEALTH_UP:
			update_health_up(&_ents[i]);
			break;
		case TYPE_JUMP_UP:
			update_jump_up(&_ents[i]);
			break;
		case TYPE_SHRINK_TOKEN:
			update_shrink_token(&_ents[i]);
			break;
		case TYPE_SPEICAL_ZONE_PORTAL:
			update_speical_zone_portal(&_ents[i]);
			break;
		case TYPE_IDOL:
			update_idol(&_ents[i]);
			break;
		}
	}
}

void update_visual_ents()
{
	for (int i = 0; i < ENT_VISUAL_COUNT; i++)
	{
		switch (_visual_ents[i].type)
		{
		case TYPE_NONE:
		case TYPE_VISUAL_SPEED_LEVEL:
		case TYPE_VISUAL_SCORE:
		case TYPE_VISUAL_JUMP_LEVEL:
			break;
		case TYPE_VISUAL_SPEED_LINE:
			update_speed_line(&_visual_ents[i]);
			break;
		case TYPE_VISUAL_LIFE:
			update_life_display(get_player_life());
			break;
		case TYPE_VISUAL_ENEMY_BISUCT_DEATH:
			update_enemy_biscut_death(&_visual_ents[i]);
			break;
		case TYPE_VISUAL_ENEMY_BISUCT_UFO_DEATH:
			update_enemy_ufo_biscut_death(&_visual_ents[i]);
			break;
		case TYPE_VISUAL_SPLASH:
			update_splash_effect(&_visual_ents[i]);
			break;
		}
	}
}