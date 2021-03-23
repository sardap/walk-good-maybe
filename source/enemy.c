#include "enemy.h"

#include <tonc.h>
#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

#include "common.h"
#include "anime.h"
#include "graphics.h"
#include "debug.h"
#include "ent.h"

#include "assets/toast_enemy_idle_01.h"
#include "assets/toast_enemy_idle_02.h"
#include "assets/toast_enemy_idle_03.h"
#include "assets/toast_enemy_idle_04.h"
#include "assets/toast_enemy_idle_05.h"
#include "assets/enemyBisuctDeath00.h"
#include "assets/enemyBisuctDeath01.h"
#include "assets/enemyBisuctDeath02.h"
#include "assets/enemyBisuctDeath03.h"

const uint *enemy_biscut_idle_cycle[] = {
	toast_enemy_idle_01Tiles, toast_enemy_idle_01Tiles, toast_enemy_idle_02Tiles,
	toast_enemy_idle_03Tiles, toast_enemy_idle_04Tiles, toast_enemy_idle_05Tiles};

const uint *enemy_biscut_death_cycle[] = {
	enemyBisuctDeath00Tiles,
	enemyBisuctDeath01Tiles,
	enemyBisuctDeath02Tiles,
	enemyBisuctDeath03Tiles};

void create_enemy_biscut(ent_t *ent, int att_idx, FIXED x, FIXED y)
{
	ent->ent_type = TYPE_ENEMY;

	ent->x = x;
	ent->w = 8;
	ent->y = y;
	ent->h = 16;
	ent->vx = 0;
	ent->vy = 0;
	ent->ent_idx = att_idx;

	//Load tiles
	ent->eb_tile_id = allocate_obj_tile_idx(2);
	GRIT_CPY(&tile_mem[4][ent->eb_tile_id], toast_enemy_idle_01Tiles);

	ent->eb_anime_cycle = 0;

	ent->att.attr0 = ATTR0_TALL | ATTR0_8BPP;
	ent->att.attr1 = ATTR1_SIZE_16x8;
	ent->att.attr2 = ATTR2_PRIO(1) | ATTR2_ID(ent->eb_tile_id);
}

void update_enemy_biscut(ent_t *ent)
{
	if (frame_count() % 2)
	{
		step_anime_bad(
			enemy_biscut_idle_cycle, toast_enemy_idle_01TilesLen, ENEMY_BISCUT_IDLE_CYCLE,
			&ent->eb_anime_cycle, ent->eb_tile_id);
	}

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

			int visual_ent_idx = allocate_visual_ent(1);
			create_enemy_biscut_death(
				&_visual_ents[visual_ent_idx], visual_ent_idx,
				ent->x, ent->y);
		}

		free_obj_tile_idx(ent->eb_tile_id, 2);
		free_ent(ent->ent_idx, 1);
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
}

void create_enemy_biscut_death(visual_ent_t *v_ent, int ent_idx, FIXED x, FIXED y)
{
	v_ent->type = TYPE_VISUAL_ENEMY_BISUCT_DEATH;

	v_ent->x = x;
	v_ent->y = y;
	v_ent->ent_idx = ent_idx;

	v_ent->eb_anime_cycle = 0;

	//Load tiles
	v_ent->eb_tile_id = allocate_obj_tile_idx(2);
	GRIT_CPY(&tile_mem[4][v_ent->eb_tile_id], enemyBisuctDeath00Tiles);

	v_ent->att.attr0 = ATTR0_TALL | ATTR0_8BPP;
	v_ent->att.attr1 = ATTR1_SIZE_8x16;
	v_ent->att.attr2 = ATTR2_PRIO(0) | ATTR2_ID(v_ent->eb_tile_id);
}

void update_enemy_biscut_death(visual_ent_t *v_ent)
{
	//God I wish I wrote comments explaning this
	if (frame_count() % 6 == 0)
	{
		bool anime_complete = step_anime(
			&v_ent->eb_anime_cycle,
			enemy_biscut_death_cycle, ENEMY_BISCUT_DEATH_CYCLE,
			v_ent->eb_tile_id, enemyBisuctDeath00TilesLen);

		if (anime_complete)
		{
			free_obj_tile_idx(v_ent->eb_tile_id, 2);
			free_visual_ent(v_ent->visual_ent_idx, 1);
			v_ent->type = TYPE_VISUAL_NONE;
			return;
		}
	}

	v_ent->x += -_scroll_x;
}
