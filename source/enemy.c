#include "enemy.h"

#include <tonc.h>
#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

#include "common.h"
#include "anime.h"
#include "graphics.h"
#include "debug.h"

#include "assets/toast_enemy_idle_01.h"
#include "assets/toast_enemy_idle_02.h"
#include "assets/toast_enemy_idle_03.h"
#include "assets/toast_enemy_idle_04.h"
#include "assets/toast_enemy_idle_05.h"

const uint *enemy_toast_idle_cycle[] = {
	toast_enemy_idle_01Tiles, toast_enemy_idle_01Tiles, toast_enemy_idle_02Tiles,
	toast_enemy_idle_03Tiles, toast_enemy_idle_04Tiles, toast_enemy_idle_05Tiles};

static int _enemy_tile_idx;
static int _enemy_anime_cycle = 0;

void load_enemy_toast()
{
	_enemy_tile_idx = allocate_obj_tile_idx(2);
	dma3_cpy(&tile_mem[4][_enemy_tile_idx], toast_enemy_idle_01Tiles, toast_enemy_idle_01TilesLen);
}

void create_toast_enemy(ent_t *ent, int att_idx, FIXED x, FIXED y)
{
	ent->ent_type = TYPE_ENEMY;

	ent->x = x;
	ent->w = 8;
	ent->y = y;
	ent->h = 16;
	ent->vx = 0;
	ent->vy = 0;
	ent->att_idx = att_idx;

	obj_set_attr(
		get_ent_att(ent),
		ATTR0_TALL | ATTR0_8BPP, ATTR1_SIZE_16x8,
		ATTR2_PRIO(1) | ATTR2_ID(_enemy_tile_idx));

	obj_set_pos(get_ent_att(ent), fx2int(ent->x), fx2int(ent->y));
}

void step_enemy_global()
{
	if (frame_count() % 2)
	{
		step_anime(
			enemy_toast_idle_cycle, toast_enemy_idle_01TilesLen, ENEMY_TOAST_IDLE_CYCLE,
			&_enemy_anime_cycle, _enemy_tile_idx);
	}
}

void update_enemy(ent_t *ent)
{
	if (ent->x + ent->w < 0 || ent->ent_cols & (TYPE_BULLET))
	{
		if (ent->ent_cols & (TYPE_BULLET))
		{
			//Play sound
			mm_sound_effect damage = {
				{SFX_PD_ENEMY_0_DEATH_0},
				(int)(1.0f * (1 << 10)),
				0,
				120,
				127,
			};
			mmEffectEx(&damage);
		}

		free_ent(ent->att_idx, 1);
		ent->ent_type = TYPE_NONE;
		return;
	}

	bool hit_y = ent_move_y(ent, ent->vy);
	// Applies gravity
	if (!hit_y)
	{
		if (ent->vy < TERMINAL_VY)
		{
			ent->vy += GRAVITY;
		}
		ent->vx = 0;
	}
	else
	{
		if (ent->vx == 0)
		{
			ent->vx = float2fx(0.75);
		}
		int hit_x = ent_level_collision_at(ent, ent->vx, ent->vy);
		if (!hit_x)
		{
			ent->vx = -ent->vx;
		}
	}

	ent->vx += -_scroll_x;
	ent_move_x(ent, ent->vx);
	//Take back scroll for next loop
	ent->vx += _scroll_x;

	obj_set_pos(get_ent_att(ent), fx2int(ent->x), fx2int(ent->y));
}
