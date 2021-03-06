#include "enemy.h"

#include <tonc.h>
#include <maxmod.h>

#include "common.h"
#include "anime.h"
#include "graphics.h"
#include "debug.h"
#include "ent.h"
#include "sound.h"

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

#include "assets/enemyBiscutUFODeath00.h"
#include "assets/enemyBiscutUFODeath01.h"
#include "assets/enemyBiscutUFODeath02.h"
#include "assets/enemyBiscutUFODeath03.h"
#include "assets/enemyBiscutUFODeath04.h"
#include "assets/enemyBiscutUFODeath05.h"

#include "assets/enemyBullet00.h"

const uint *enemy_biscut_idle_cycle[] = {
	toast_enemy_idle_01Tiles, toast_enemy_idle_02Tiles,
	toast_enemy_idle_03Tiles, toast_enemy_idle_04Tiles,
	toast_enemy_idle_05Tiles};

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

const uint *enemy_biscut_ufo_death_cycle[] = {
	enemyBiscutUFODeath00Tiles,
	enemyBiscutUFODeath01Tiles,
	enemyBiscutUFODeath02Tiles,
	enemyBiscutUFODeath03Tiles,
	enemyBiscutUFODeath03Tiles,
	enemyBiscutUFODeath03Tiles,
	enemyBiscutUFODeath04Tiles,
	enemyBiscutUFODeath04Tiles,
	enemyBiscutUFODeath04Tiles,
	enemyBiscutUFODeath05Tiles,
	enemyBiscutUFODeath05Tiles,
	enemyBiscutUFODeath05Tiles,
};

static int _enemy_bullet_0_tile_idx;

void load_enemy_bullets_tiles()
{
	_enemy_bullet_0_tile_idx = allocate_obj_tile_idx(1);
	GRIT_CPY(&tile_mem[4][_enemy_bullet_0_tile_idx], enemyBullet00Tiles);
}

void free_enemy_bullets_tiles()
{
	free_obj_tile_idx(_enemy_bullet_0_tile_idx, 1);
}

void create_enemy_biscut(ent_t *ent, FIXED x, FIXED y)
{
	ent->ent_type = TYPE_ENEMY_BISCUT;

	ent->x = x;
	ent->w = 8;
	ent->y = y;
	ent->h = 16;
	ent->vx = 0;
	ent->vy = 0;

	//Load tiles
	ent->eb_tile_idx = allocate_obj_tile_idx(2);
	GRIT_CPY(&tile_mem[4][ent->eb_tile_idx], toast_enemy_idle_01Tiles);

	ent->eb_anime_cycle = 0;

	ent->att.attr0 = ATTR0_TALL | ATTR0_8BPP;
	ent->att.attr1 = ATTR1_SIZE_16x8;
	ent->att.attr2 = ATTR2_PRIO(1) | ATTR2_ID(ent->eb_tile_idx);
}

void update_enemy_biscut(ent_t *ent)
{
	if (frame_count() % 6 == 0)
	{
		step_anime(
			&ent->eb_anime_cycle,
			enemy_biscut_idle_cycle, ENEMY_BISCUT_IDLE_CYCLE,
			ent->eb_tile_idx, toast_enemy_idle_01TilesLen);
	}

	if (ent->x + ent->w < 0 || ent->ent_cols & (TYPE_BULLET))
	{
		if (ent->ent_cols & (TYPE_BULLET))
		{
			add_score(ENEMY_BISCUT_SCORE);

			//Play sound
			mmEffectEx(&_enemy_biscut_damage);

			visual_ent_t *visual_ent = allocate_visual_ent();
			create_enemy_biscut_death(
				visual_ent,
				ent->x, ent->y);
		}

		free_obj_tile_idx(ent->eb_tile_idx, 2);
		free_ent(ent);
		return;
	}

	bool hit_y = ent_move_y(ent, ent->vy);
	// Applies gravity
	if (!hit_y)
	{
		if (ent->vy < TERMINAL_VY)
			ent->vy += GRAVITY;

		ent->vx = 0;
	}
	else
	{
		if (ent->vx == 0)
			ent->vx = 0.75 * FIX_SCALEF;

		if (!ent_level_collision_at(ent, ent->vx, ent->vy))
		{
			ent->vx = -ent->vx;
		}
	}

	ent->vx += -_scroll_x;
	ent_move_x(ent, ent->vx);
	//Take back scroll for next loop
	ent->vx += _scroll_x;

	obj_set_pos(&ent->att, fx2int(ent->x), fx2int(ent->y));
}

void create_enemy_biscut_death(visual_ent_t *v_ent, FIXED x, FIXED y)
{
	v_ent->type = TYPE_VISUAL_ENEMY_BISUCT_DEATH;

	v_ent->x = x;
	v_ent->y = y;

	v_ent->eb_anime_cycle = 0;

	//Load tiles
	v_ent->eb_tile_idx = allocate_obj_tile_idx(2);
	GRIT_CPY(&tile_mem[4][v_ent->eb_tile_idx], enemyBisuctDeath00Tiles);

	v_ent->att.attr0 = ATTR0_TALL | ATTR0_8BPP;
	v_ent->att.attr1 = ATTR1_SIZE_8x16;
	v_ent->att.attr2 = ATTR2_PRIO(0) | ATTR2_ID(v_ent->eb_tile_idx);
}

void update_enemy_biscut_death(visual_ent_t *v_ent)
{
	if (frame_count() % 6 == 0)
	{
		bool anime_complete = step_anime(
			&v_ent->eb_anime_cycle,
			enemy_biscut_death_cycle, ENEMY_BISCUT_DEATH_CYCLE,
			v_ent->eb_tile_idx, enemyBisuctDeath00TilesLen);

		if (anime_complete)
		{
			free_obj_tile_idx(v_ent->eb_tile_idx, 2);
			free_visual_ent(v_ent);
			return;
		}
	}

	v_ent->x += -_scroll_x;
}

void create_enemy_ufo_bisuct(ent_t *ent, FIXED x, FIXED y)
{
	ent->ent_type = TYPE_ENEMY_BISCUT_UFO;

	ent->x = x;
	ent->w = 16;
	ent->y = y;
	ent->h = 16;
	ent->vx = 0;
	ent->vy = 0;

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
		add_score(ENEMY_UFO_BISCUT_SCORE);

		//Play death sound
		mmEffectEx(&_enemy_ufo_damge);

		free_obj_tile_idx(ent->ebu_tile_id, 4);
		free_ent(ent);

		visual_ent_t *v_ent = allocate_visual_ent();
		create_enemy_ufo_biscut_death(v_ent, ent->x, ent->y);
		return;
	}

	--ent->ebu_next_shoot;

	if (ent->ebu_next_shoot <= 0)
	{
		ent->ebu_next_shoot = 60;

		ent_t *bul_ent = allocate_ent();
		create_enemy_bullet(
			bul_ent, ent->x + 6 * FIX_SCALE,
			ent->y + 16 * FIX_SCALE, 0, 0.5 * FIX_SCALE);

		mmEffectEx(&_enemy_shoot);
	}

	obj_set_pos(&ent->att, fx2int(ent->x), fx2int(ent->y));
}

void create_enemy_ufo_biscut_death(visual_ent_t *v_ent, FIXED x, FIXED y)
{
	v_ent->type = TYPE_VISUAL_ENEMY_BISUCT_UFO_DEATH;

	v_ent->x = x;
	v_ent->y = y;

	v_ent->eb_anime_cycle = 0;

	//Load tiles
	v_ent->eb_tile_idx = allocate_obj_tile_idx(4);
	GRIT_CPY(&tile_mem[4][v_ent->eb_tile_idx], enemyBisuctDeath00Tiles);

	v_ent->att.attr0 = ATTR0_SQUARE | ATTR0_8BPP;
	v_ent->att.attr1 = ATTR1_SIZE_16x16;
	v_ent->att.attr2 = ATTR2_PRIO(0) | ATTR2_ID(v_ent->eb_tile_idx);
}

void update_enemy_ufo_biscut_death(visual_ent_t *v_ent)
{

	if (frame_count() % 3 == 0)
	{
		bool anime_complete = step_anime(
			&v_ent->eb_anime_cycle,
			enemy_biscut_ufo_death_cycle, ENEMY_BISCUT_UFO_DEATH_CYCLE,
			v_ent->eb_tile_idx, enemyBiscutUFODeath00TilesLen);

		if (anime_complete)
		{
			free_obj_tile_idx(v_ent->eb_tile_idx, 2);
			free_visual_ent(v_ent);
			return;
		}
	}

	v_ent->x += -_scroll_x;
}

void create_enemy_bullet(ent_t *ent, FIXED x, FIXED y, FIXED vx, FIXED vy)
{
	ent->ent_type = TYPE_ENEMY_BULLET;

	ent->x = x;
	ent->w = 6;
	ent->y = y;
	ent->h = 5;
	ent->vx = vx;
	ent->vy = vy;

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
		free_ent(ent);
		return;
	}

	obj_set_pos(&ent->att, fx2int(ent->x), fx2int(ent->y));
}
