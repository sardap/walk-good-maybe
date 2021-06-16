#include "obstacles.h"

#include <maxmod.h>
#include "soundbank.h"

#include "common.h"
#include "player.h"
#include "graphics.h"
#include "ent.h"
#include "sound.h"

#include "assets/speedUp.h"
#include "assets/speedLine.h"
#include "assets/healthUp.h"
#include "assets/jumpUp.h"
#include "assets/shrinkToken.h"

static int _speed_up_tile_idx;
static int _speed_lines_idx;
static int _health_up_tile_idx;
static int _jump_up_tile_idx;
static int _shrink_token_tile_idx;

void load_speed_up()
{
	_speed_up_tile_idx = allocate_obj_tile_idx(1);
	GRIT_CPY(&tile_mem[4][_speed_up_tile_idx], speedUpTiles);
}

void free_speed_up()
{
	free_obj_tile_idx(_speed_up_tile_idx, 1);
}

void load_health_up()
{
	_health_up_tile_idx = allocate_obj_tile_idx(1);
	GRIT_CPY(&tile_mem[4][_health_up_tile_idx], healthUpTiles);
}

void free_health_up()
{
	free_obj_tile_idx(_health_up_tile_idx, 1);
}

void load_jump_up()
{
	_jump_up_tile_idx = allocate_obj_tile_idx(1);
	GRIT_CPY(&tile_mem[4][_jump_up_tile_idx], jumpUpTiles);
}

void free_jump_up()
{
	free_obj_tile_idx(_jump_up_tile_idx, 1);
}

void load_shrink_token()
{
	_shrink_token_tile_idx = allocate_obj_tile_idx(1);
	GRIT_CPY(&tile_mem[4][_shrink_token_tile_idx], shrinkTokenTiles);
}

void free_shrink_token()
{
	free_obj_tile_idx(_shrink_token_tile_idx, 1);
}

void create_speed_up(ent_t *ent, FIXED x, FIXED y)
{
	ent->ent_type = TYPE_SPEED_UP;

	ent->x = x;
	ent->w = 8;
	ent->y = y;
	ent->h = 8;
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
		if (ent->ent_cols & (TYPE_PLAYER))
			mmEffectEx(&_speed_pick_up_sound);

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

	obj_set_pos(&ent->att, fx2int(ent->x), fx2int(ent->y));
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
	ent->w = 8;
	ent->y = y;
	ent->h = 8;
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
		if (ent->ent_cols & (TYPE_PLAYER))
			mmEffectEx(&_health_pick_up_sound);

		free_ent(ent);
		return;
	}

	ent->vx += -_scroll_x;
	ent_move_x_dirty(ent);
	//Take back scroll for next loop
	ent->vx += _scroll_x;

	obj_set_pos(&ent->att, fx2int(ent->x), fx2int(ent->y));
}

void create_jump_up(ent_t *ent, FIXED x, FIXED y)
{
	ent->ent_type = TYPE_JUMP_UP;

	ent->x = x;
	ent->w = 8;
	ent->y = y;
	ent->h = 8;
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
		if (ent->ent_cols & (TYPE_PLAYER))
			mmEffectEx(&_jump_pick_up_sound);

		free_ent(ent);
		return;
	}

	ent->vx += -_scroll_x;
	ent_move_x_dirty(ent);
	//Take back scroll for next loop
	ent->vx += _scroll_x;

	obj_set_pos(&ent->att, fx2int(ent->x), fx2int(ent->y));
}

void create_shrink_token(ent_t *ent, FIXED x, FIXED y)
{
	ent->ent_type = TYPE_SHRINK_TOKEN;

	ent->x = x;
	ent->w = 4;
	ent->y = y;
	ent->h = 4;
	ent->vx = 0;
	ent->vy = 0;

	ent->att.attr0 = ATTR0_SQUARE | ATTR0_8BPP;
	ent->att.attr1 = ATTR1_SIZE_8x8;
	ent->att.attr2 = ATTR2_PRIO(0) | ATTR2_ID(_shrink_token_tile_idx);
}

void update_shrink_token(ent_t *ent)
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

	obj_set_pos(&ent->att, fx2int(ent->x), fx2int(ent->y));
}
