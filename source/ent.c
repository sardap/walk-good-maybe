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
ent_t _ents[ENT_COUNT];

static int _att_count = 0;

static int _allocated_objs[128];

void init_obj_atts()
{
	oam_init(_obj_buffer, 128);

	for (int i = 0; i < 128; i++)
	{
		_allocated_objs[i] = 0;
	}
	_allocated_objs[0] = 1;
}

int allocate_ent(int count)
{
	for (int i = 0; i < 128;)
	{
		bool found = true;

		for (int j = i; j - i < count && j < 128; j++)
		{
			if (_allocated_objs[j])
			{
				found = false;
				break;
			}
		}

		if (found)
		{
			for (int j = i; j - i < count; j++)
			{
				_allocated_objs[j] = 1;
			}
			if (i > _att_count)
			{
				_att_count = i + count;
			}
			return i;
		}
		i += count;
	}

	return -1;
}

void free_ent(int idx, int count)
{
	if (idx >= _att_count)
	{
		_att_count -= count;
	}

	for (int i = idx; i < idx + count; i++)
	{
		_allocated_objs[i] = 0;
	}
}

void copy_ents_to_oam()
{
	int obj_idx = 0;

	ent_t *ent;

	for (int i = 0; i < ENT_COUNT; i++)
	{
		ent = &_ents[i];

		if (ent->ent_type == TYPE_NONE)
			continue;

		_obj_buffer[obj_idx] = ent->att;
		obj_set_pos(&_obj_buffer[obj_idx], fx2int(ent->x), fx2int(ent->y));

		++obj_idx;
	}

	for (int i = obj_idx; i < 128; i++)
	{
		obj_set_attr(&_obj_buffer[i], ATTR0_HIDE, 0, 0);
	}

	oam_copy(oam_mem, _obj_buffer, 128);
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
		case TYPE_SPEED_LINE:
			update_speed_line(&_ents[i]);
			break;
		case TYPE_SCORE:
		case TYPE_LIFE:
			update_life_display(get_player_life());
			break;
		}
	}

	step_enemy_global();
}