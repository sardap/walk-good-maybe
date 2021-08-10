#include "effect.h"

#include <tonc.h>

#include "graphics.h"
#include "common.h"
#include "anime.h"
#include "player.h"

#include "assets/exSplash00.h"
#include "assets/exSplash01.h"
#include "assets/exSplash02.h"
#include "assets/exSplash03.h"
#include "assets/exSplash04.h"

static const int splash_anime_length = 10;
static const uint *splash_anime[] = {
	exSplash00Tiles,
	exSplash01Tiles,
	exSplash02Tiles, exSplash02Tiles,
	exSplash03Tiles, exSplash03Tiles, exSplash03Tiles,
	exSplash04Tiles, exSplash04Tiles,
	exSplash00Tiles};

void create_splash_effect(visual_ent_t *v_ent, FIXED x, FIXED y)
{
	v_ent->type = TYPE_VISUAL_SPLASH;

	v_ent->x = x;
	v_ent->y = y;

	v_ent->sp_anime_cycle = 0;

	//Load tiles
	v_ent->sp_tile_idx = allocate_obj_tile_idx(4);
	GRIT_CPY(&tile_mem[4][v_ent->sp_tile_idx], exSplash00Tiles);

	v_ent->att.attr0 = ATTR0_WIDE | ATTR0_8BPP;
	v_ent->att.attr1 = ATTR1_SIZE_32x16;
	v_ent->att.attr2 = ATTR2_PRIO(PLAYER_PRIO - 1) | ATTR2_ID(v_ent->sp_tile_idx);
}

void update_splash_effect(visual_ent_t *v_ent)
{
	if (frame_count() % 3 == 0)
	{
		BOOL anime_complete = step_anime(
			&v_ent->sp_anime_cycle,
			splash_anime, splash_anime_length,
			v_ent->eb_tile_idx, exSplash00TilesLen);

		if (anime_complete)
		{
			free_splash_effect(v_ent);
			return;
		}
	}

	v_ent->x += -_scroll_x;
}

void free_splash_effect(visual_ent_t *v_ent)
{
	free_obj_tile_idx(v_ent->sp_tile_idx, 4);
	free_visual_ent(v_ent);
}