#include "obstacles.h"

#include "common.h"
#include "player.h"
#include "graphics.h"

#include "assets/speedUp.h"
#include "assets/speedLine.h"

static int _speed_up_tile_idx;
static int _speed_lines_idx;

void load_speed_up()
{
	_speed_up_tile_idx = allocate_obj_tile_idx(1);
	dma3_cpy(&tile_mem[4][_speed_up_tile_idx], speedUpTiles, speedUpTilesLen);

	_speed_lines_idx = allocate_obj_tile_idx(4);
	dma3_cpy(&tile_mem[4][_speed_lines_idx], speedLineTiles, speedLineTilesLen);
}

void create_speed_up(ent_t *ent, int att_idx, FIXED x, FIXED y)
{
	ent->ent_type = TYPE_SPEED_UP;

	ent->x = x;
	//This is inccorect so it fails fast for the col dect
	ent->w = 0;
	ent->y = y;
	ent->h = 0;
	ent->vx = 0;
	ent->vy = 0;
	ent->ent_idx = att_idx;

	obj_set_attr(
		get_ent_att(ent),
		ATTR0_SQUARE | ATTR0_8BPP, ATTR1_SIZE_8x8,
		ATTR2_PRIO(1) | ATTR2_ID(_speed_up_tile_idx));

	obj_set_pos(get_ent_att(ent), fx2int(ent->x), fx2int(ent->y));
}

void create_speed_line(ent_t *ent, int att_idx, FIXED x, FIXED y)
{
	ent->ent_type = TYPE_SPEED_LINE;

	ent->x = x;
	ent->w = 32;
	ent->y = y;
	ent->h = 8;
	ent->vx = _scroll_x + -3.5f * FIX_SCALEF;
	ent->vy = 0;
	ent->ent_idx = att_idx;

	obj_set_attr(
		get_ent_att(ent),
		ATTR0_WIDE | ATTR0_8BPP, ATTR1_SIZE_32x8,
		ATTR2_PRIO(2) | ATTR2_ID(_speed_lines_idx));

	obj_set_pos(get_ent_att(ent), fx2int(ent->x), fx2int(ent->y));
}

void update_speed_up(ent_t *ent)
{
	if (ent->x + ent->w < 0 || (ent->ent_cols & (TYPE_PLAYER) && !speed_up_active()))
	{
		free_ent(ent->ent_idx, 1);
		ent->ent_type = TYPE_NONE;
		int count = gba_rand_range(3, 7);
		int line_ent = allocate_ent(count);
		for (int i = 0; i < count; i++)
		{
			create_speed_line(
				&_ents[line_ent + i], line_ent + i,
				GBA_WIDTH * FIX_SCALE + gba_rand_range(0, GBA_WIDTH * FIX_SCALE),
				int2fx(gba_rand_range(0, GBA_HEIGHT)));
		}
		return;
	}

	ent->vx += -_scroll_x;
	ent_move_x_dirty(ent);
	//Take back scroll for next loop
	ent->vx += _scroll_x;

	obj_set_pos(get_ent_att(ent), fx2int(ent->x), fx2int(ent->y));
}

void update_speed_line(ent_t *ent)
{
	if (ent->x < GBA_WIDTH * FIX_SCALE && !speed_up_active())
	{
		free_ent(ent->ent_idx, 1);
		ent->ent_type = TYPE_NONE;
		return;
	}

	ent_move_x_dirty(ent);

	obj_set_pos(get_ent_att(ent), fx2int(ent->x), fx2int(ent->y));
}