#include "ent.h"

#include <tonc.h>

#include "common.h"
#include "level.h"
#include "debug.h"
#include "obstacles.h"
#include "gun.h"
#include "enemy.h"
#include "player.h"
#include "life_display.h"

OBJ_ATTR _obj_buffer[128] = {};
ent_t _ents[ENT_COUNT] = {};
visual_ent_t _visual_ents[ENT_VISUAL_COUNT] = {};

static int _allocated_ents[ENT_COUNT];
static int _allocated_visual_ents[ENT_VISUAL_COUNT];

void init_all_ents()
{
	oam_init(_obj_buffer, 128);

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

static int allocate(int *ary, int length, int count)
{
	for (int i = 0; i < length;)
	{
		bool found = true;

		for (int j = i; j - i < count && j < length; j++)
		{
			if (ary[j])
			{
				found = false;
				break;
			}
		}

		if (found)
		{
			for (int j = i; j - i < count; j++)
			{
				ary[j] = 1;
			}
			return i;
		}
		i += count;
	}

	return -1;
}

int allocate_ent(int count)
{
	return allocate(_allocated_ents, ENT_COUNT, count);
}

void free_ent(int idx, int count)
{
	for (int i = idx; i < idx + count; i++)
	{
		_allocated_ents[i] = 0;
	}
}

int allocate_visual_ent(int count)
{
	return allocate(_allocated_visual_ents, ENT_VISUAL_COUNT, count);
}

void free_visual_ent(int idx, int count)
{
	for (int i = idx; i < idx + count; i++)
	{
		_allocated_ents[i] = 0;
	}
}

void copy_ents_to_oam()
{
	int obj_idx = 0;

	for (int i = 0; i < ENT_COUNT; i++)
	{
		ent_t *ent = &_ents[i];

		if (ent->ent_type == TYPE_NONE)
			continue;

		_obj_buffer[obj_idx] = ent->att;
		obj_set_pos(&_obj_buffer[obj_idx], fx2int(ent->x), fx2int(ent->y));

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

	oam_copy(oam_mem, _obj_buffer, ENT_COUNT + ENT_VISUAL_COUNT);
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

// STOLEN from https://github.com/exelotl/goodboy-advance
bool ent_move_x(ent_t *e, FIXED dx)
{
	if (did_hit_x(e, dx))
	{
		int sign = dx >= 0 ? 1 : -1;
		while (fx2int(dx) != 0 && !did_hit_x(e, sign))
		{
			e->x += sign;
			dx -= sign;
		}

		//NOT STOLEN MY OWN SHIT CODE
		//Failed to push out
		if (did_hit_x(e, 0))
		{
			int start = (translate_x(e) / FIX_SCALE) / TILE_WIDTH;
			int y = (translate_y(e) / FIX_SCALE) / TILE_WIDTH;

			//Failed to push out but will only check all tiles
			for (int i = 0; i < 32; i++)
			{
				//Left
				if (!tile_to_collision(at_level(level_wrap_x(start - i), y)))
				{
					while (did_hit_x(e, 0))
					{
						e->x--;
					}
					break;
				}

				//Right
				if (!tile_to_collision(at_level(level_wrap_x(start + i), y)))
				{
					while (did_hit_x(e, 0))
					{
						e->x++;
					}
					break;
				}
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
		int sign = dy >= 0 ? 1 : -1;
		while (fx2int(dy) != 0 && !did_hit_y(e, sign))
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

bool apply_gravity(ent_t *e)
{
	int flags = ent_level_collision_at(e, 0, GRAVITY);

	if (flags & (LEVEL_COL_GROUND))
	{
		_player.vy = 0;
		return true;
	}

	_player.vy += GRAVITY;
	return false;
}

void visual_ent_move_x(visual_ent_t *e)
{
	e->x += e->vx;
}

void visual_ent_move_Y(visual_ent_t *e)
{
	e->y += e->vy;
}

void update_ents()
{
	//Add player to ent array
	_ents[ENT_COUNT - 1] = _player;

	for (int i = 0; i < ENT_COUNT; i++)
	{
		if (_ents[i].ent_type == TYPE_NONE)
			continue;

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
				_ents[i]
					.ent_cols |= _ents[j].ent_type;
			}
		}
	}

	//Copy updated player ent back into player
	_player = _ents[ENT_COUNT - 1];

	for (int i = 0; i < ENT_COUNT; i++)
	{
		switch (_ents[i].ent_type)
		{
		case TYPE_BULLET:
			update_bullet(&_ents[i]);
			break;
		case TYPE_ENEMY:
			update_enemy(&_ents[i]);
			break;
		case TYPE_PLAYER:
			break;
		case TYPE_NONE:
			break;
		case TYPE_SPEED_UP:
			update_speed_up(&_ents[i]);
			break;
		}
	}

	step_enemy_global();
}

void update_visual_ents()
{
	for (int i = 0; i < ENT_VISUAL_COUNT; i++)
	{
		switch (_visual_ents[i].type)
		{
		case TYPE_VISUAL_SPEED_LINE:
			update_speed_line(&_visual_ents[i]);
			break;
		case TYPE_VISUAL_LIFE:
			update_life_display(get_player_life());
			break;
		case TYPE_VISUAL_SCORE:
			break;
		}
	}
}