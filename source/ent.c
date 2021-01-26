#include "ent.h"

#include <tonc.h>

#include "common.h"
#include "level.h"
#include "debug.h"

#include "gun.h"
#include "enemy.h"

OBJ_ATTR _obj_buffer[128] = {};
FIXED _bg_pos_x = 0;
ent_t _ents[ENT_COUNT];
static int _att_count;
static int _free_obj;

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

int allocate_att(int count)
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

	return 0;
}

void free_att(int count, int idx)
{
	obj_set_attr(&_obj_buffer[idx], ATTR0_HIDE, 0, 0);

	if (idx >= _att_count)
	{
		_att_count -= count;
	}

	for (int i = idx; i < idx + count; i++)
	{
		_allocated_objs[i] = 0;
	}

	_free_obj++;
}

//THIS SHOULD ONLY BE CALLED ONCE A LOOP
int att_count()
{
	int result = _free_obj + (_att_count + 1);
	_free_obj = 0;
	return result;
}

FIXED translate_x(ent_t *e)
{
	return e->x + _bg_pos_x;
}

FIXED translate_y(ent_t *e)
{
	return e->y;
}

static inline FIXED level_to_screen(int l)
{
	return int2fx(l * TILE_WIDTH + l % TILE_WIDTH);
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
	for (int i = 0; i < ENT_COUNT; i++)
	{
		if (_ents[i].ent_type == TYPE_NONE)
		{
			continue;
		}

		_ents[i].ent_cols = TYPE_NONE;
		write_to_log(LOG_LEVEL_INFO, "COL LOOKING");

		for (int j = 0; j < ENT_COUNT; j++)
		{
			if (
				i != j &&
				_ents[j].ent_type != TYPE_NONE &&
				_ents[i].x < _ents[j].x + _ents[j].w &&
				_ents[i].x + _ents[i].w > _ents[j].x &&
				_ents[i].y < _ents[j].y + _ents[j].h &&
				_ents[i].y + _ents[i].h < _ents[j].y)
			{
				_ents[i].ent_cols &= _ents[j].ent_type;
				write_to_log(LOG_LEVEL_INFO, "COL FOUND");
			}
		}
	}

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
		}
	}

	step_enemy_global();
}