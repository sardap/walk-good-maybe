#include "obstacles.h"

#include "common.h"
#include "player.h"
#include "graphics.h"
#include "ent.h"

#include "assets/speedUp.h"
#include "assets/speedLine.h"
#include "assets/healthUp.h"
#include "assets/jumpUp.h"

static int _speed_up_tile_idx;
static int _speed_lines_idx;
static int _health_up_tile_idx;
static int _jump_up_tile_idx;

void load_speed_up()
{
	_speed_up_tile_idx = allocate_obj_tile_idx(1);
	GRIT_CPY(&tile_mem[4][_speed_up_tile_idx], speedUpTiles);

	_speed_lines_idx = allocate_obj_tile_idx(4);
	GRIT_CPY(&tile_mem[4][_speed_lines_idx], speedLineTiles);
}

void load_health_up()
{
	_health_up_tile_idx = allocate_obj_tile_idx(1);
	GRIT_CPY(&tile_mem[4][_health_up_tile_idx], healthUpTiles);
}

void load_jump_up()
{
	_jump_up_tile_idx = allocate_obj_tile_idx(1);
	GRIT_CPY(&tile_mem[4][_jump_up_tile_idx], jumpUpTiles);
}

void create_speed_up(ent_t *ent, FIXED x, FIXED y)
{
	ent->ent_type = TYPE_SPEED_UP;

	ent->x = x;
	ent->w = 0;
	ent->y = y;
	ent->h = 0;
	ent->vx = 0;
	ent->vy = 0;

	ent->att.attr0 = ATTR0_SQUARE | ATTR0_8BPP;
	ent->att.attr1 = ATTR1_SIZE_8x8;
	ent->att.attr2 = ATTR2_PRIO(2) | ATTR2_ID(_speed_up_tile_idx);
}

void update_speed_up(ent_t *ent)
{
	if (ent->x + ent->w < 0 || (ent->ent_cols & (TYPE_PLAYER) && !speed_up_active()))
	{
		free_ent(ent);

		int count = gba_rand_range(3, 5);
		for (int i = 0; i < count; i++)
		{
			visual_ent_t *speed_line_ent = allocate_visual_ent();
			create_speed_line(
				speed_line_ent,
				GBA_WIDTH * FIX_SCALE + gba_rand_range(0, GBA_WIDTH * FIX_SCALE),
				int2fx(gba_rand_range(0, GBA_HEIGHT)));
		}
		return;
	}

	ent->vx += -_scroll_x;
	ent_move_x_dirty(ent);
	//Take back scroll for next loop
	ent->vx += _scroll_x;
}

void create_speed_line(visual_ent_t *v_ent, FIXED x, FIXED y)
{
	v_ent->type = TYPE_VISUAL_SPEED_LINE;

	v_ent->sl_vx = _scroll_x + -3.5f * FIX_SCALEF;
	v_ent->x = x;
	v_ent->y = y;

	v_ent->att.attr0 = ATTR0_WIDE | ATTR0_8BPP;
	v_ent->att.attr1 = ATTR1_SIZE_32x8;
	v_ent->att.attr2 = ATTR2_PRIO(0) | ATTR2_ID(_speed_lines_idx);
}

void update_speed_line(visual_ent_t *ent)
{
	if (ent->x < GBA_WIDTH * FIX_SCALE && !speed_up_active())
	{
		free_visual_ent(ent);
		return;
	}

	ent->x += ent->sl_vx;
}

void create_health_up(ent_t *ent, FIXED x, FIXED y)
{
	ent->ent_type = TYPE_HEALTH_UP;

	ent->x = x;
	ent->w = 0;
	ent->y = y;
	ent->h = 0;
	ent->vx = 0;
	ent->vy = 0;

	ent->att.attr0 = ATTR0_SQUARE | ATTR0_8BPP;
	ent->att.attr1 = ATTR1_SIZE_8x8;
	ent->att.attr2 = ATTR2_PRIO(2) | ATTR2_ID(_health_up_tile_idx);
}

void update_health_up(ent_t *ent)
{
	if (ent->x + ent->w < 0 || ent->ent_cols & (TYPE_PLAYER))
	{
		free_ent(ent);
		return;
	}

	ent->vx += -_scroll_x;
	ent_move_x_dirty(ent);
	//Take back scroll for next loop
	ent->vx += _scroll_x;
}

void create_jump_up(ent_t *ent, FIXED x, FIXED y)
{
	ent->ent_type = TYPE_JUMP_UP;

	ent->x = x;
	ent->w = 0;
	ent->y = y;
	ent->h = 0;
	ent->vx = 0;
	ent->vy = 0;

	ent->att.attr0 = ATTR0_SQUARE | ATTR0_8BPP;
	ent->att.attr1 = ATTR1_SIZE_8x8;
	ent->att.attr2 = ATTR2_PRIO(0) | ATTR2_ID(_jump_up_tile_idx);
}

void update_jump_up(ent_t *ent)
{
	if (ent->x + ent->w < 0 || ent->ent_cols & (TYPE_PLAYER))
	{
		free_ent(ent);
		return;
	}

	ent->vx += -_scroll_x;
	ent_move_x_dirty(ent);
	//Take back scroll for next loop
	ent->vx += _scroll_x;
}