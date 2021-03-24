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
#include "assets/enemyBiscutUFOIdle00.h"
#include "assets/enemyBiscutUFOIdle01.h"
#include "assets/enemyBiscutUFOIdle02.h"
#include "assets/enemyBullet00.h"

const uint *enemy_biscut_idle_cycle[] = {
	toast_enemy_idle_01Tiles, toast_enemy_idle_01Tiles, toast_enemy_idle_02Tiles,
	toast_enemy_idle_03Tiles, toast_enemy_idle_04Tiles, toast_enemy_idle_05Tiles};

const uint *enemy_biscut_death_cycle[] = {
	enemyBisuctDeath00Tiles,
	enemyBisuctDeath01Tiles,
	enemyBisuctDeath02Tiles,
	enemyBisuctDeath03Tiles};

const uint *enemy_biscut_ufo_idle_cycle[] = {
	enemyBiscutUFOIdle00Tiles,
	enemyBiscutUFOIdle01Tiles,
	enemyBiscutUFOIdle02Tiles,
};

static int _enemy_bullet_0_tile_idx;

void load_enemy_bullets_tiles()
{
	_enemy_bullet_0_tile_idx = allocate_obj_tile_idx(1);
	GRIT_CPY(&tile_mem[4][_enemy_bullet_0_tile_idx], enemyBullet00Tiles);
}

void create_enemy_biscut(ent_t *ent, int att_idx, FIXED x, FIXED y)
{
	ent->ent_type = TYPE_ENEMY_BISCUT;

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

void create_enemy_ufo_bisuct(ent_t *ent, int ent_idx, FIXED x, FIXED y)
{
	ent->ent_type = TYPE_ENEMY_BISCUT_UFO;

	ent->x = x;
	ent->w = 16;
	ent->y = y;
	ent->h = 16;
	ent->vx = 0;
	ent->vy = 0;
	ent->ent_idx = ent_idx;

	//Load tiles
	ent->ebu_tile_id = allocate_obj_tile_idx(4);
	GRIT_CPY(&tile_mem[4][ent->ebu_tile_id], enemyBiscutUFOIdle00Tiles);

	ent->ebu_anime_cycle = 0;

	ent->ebu_next_shoot = gba_rand_range(10, 120);

	ent->att.attr0 = ATTR0_SQUARE | ATTR0_8BPP;
	ent->att.attr1 = ATTR1_SIZE_16x16;
	ent->att.attr2 = ATTR2_PRIO(1) | ATTR2_ID(ent->ebu_tile_id);
}

void update_enemy_ufo_bisuct(ent_t *ent)
{
	if (frame_count() % 6 == 0)
	{
		step_anime(
			&ent->ebu_anime_cycle,
			enemy_biscut_ufo_idle_cycle, ENEMY_BISCUT_UFO_IDLE_CYCLE,
			ent->ebu_tile_id, enemyBiscutUFOIdle00TilesLen);
	}

	ent_move_y_dirty(ent);

	ent->x += -_scroll_x;

	if (ent->x + int2fx(ent->w) < 0 || ent->ent_cols & (TYPE_BULLET))
	{
		free_obj_tile_idx(ent->ebu_tile_id, 4);
		free_ent(ent->ent_idx, 1);
		ent->ent_type = TYPE_NONE;
		return;
	}

	--ent->ebu_next_shoot;

	if (ent->ebu_next_shoot <= 0)
	{
		ent->ebu_next_shoot = 60;

		int bul_ent_idx = allocate_ent(1);
		create_enemy_bullet(
			&_ents[bul_ent_idx],
			bul_ent_idx, ent->x + 6 * FIX_SCALE,
			ent->y + 16 * FIX_SCALE, 0, 0.5 * FIX_SCALE);
	}
}

void create_enemy_bullet(ent_t *ent, int ent_idx, FIXED x, FIXED y, FIXED vx, FIXED vy)
{
	ent->ent_type = TYPE_ENEMY_BULLET;

	ent->x = x;
	ent->w = 6;
	ent->y = y;
	ent->h = 5;
	ent->vx = vx;
	ent->vy = vy;
	ent->ent_idx = ent_idx;

	ent->att.attr0 = ATTR0_SQUARE | ATTR0_8BPP;
	ent->att.attr1 = ATTR1_SIZE_8x8;
	ent->att.attr2 = ATTR2_PRIO(0) | ATTR2_ID(_enemy_bullet_0_tile_idx);
}

void update_enemy_bullet(ent_t *ent)
{
	int hit = ent->ent_cols & (TYPE_PLAYER);

	ent->vx += -_scroll_x;
	hit |= ent_move_x(ent, ent->vx);
	//Take back scroll for next loop
	ent->vx += _scroll_x;

	hit |= ent_move_y(ent, ent->vy);

	if (hit || fx2int(ent->x) + ent->w < 0)
	{
		free_ent(ent->ent_idx, 1);
		ent->ent_type = TYPE_NONE;
		return;
	}
}
