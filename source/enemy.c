#include "enemy.h"

#include <tonc.h>

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
	toast_enemy_idle_03Tiles, toast_enemy_idle_04Tiles, toast_enemy_idle_05Tiles
};

static int _enemy_top_idx = 0;
static int _enemy_tile_idx;

void load_enemy_toast() {
	_enemy_tile_idx = allocate_tile_idx(2);
	dma3_cpy(&tile_mem[4][_enemy_tile_idx], toast_enemy_idle_01Tiles, toast_enemy_idle_01TilesLen);
}


void create_toast_enemy(ent_t *ent, FIXED x, FIXED y) {
	ent->ent_type = TYPE_ENEMY;

	ent->x = x;
	ent->y = y;
	ent->vx = 0;
	ent->vy = 0;
	ent->w = 8;
	ent->h = 16;
	ent->att_idx = allocate_att(1);

	obj_set_attr(&_obj_buffer[ent->att_idx],
		ATTR0_TALL | ATTR0_8BPP, ATTR1_SIZE_8x16,
		ATTR2_PALBANK(0) | ATTR2_PRIO(1) | ATTR2_ID(_enemy_tile_idx)
	);

	obj_set_pos(&_obj_buffer[ent->att_idx], fx2int(ent->x), fx2int(ent->y));
}

void destroy_toast_enemy(ent_t *ent) {
	ent->att_idx = 0;
}

void update_enemy(ent_t *ent) {
	if(ent->x + ent->w < 0) {
		free_att(1, ent->att_idx);
		destroy_toast_enemy(ent);
		return;
	}

	step_anime(
		enemy_toast_idle_cycle, toast_enemy_idle_01TilesLen, ENEMY_TOAST_IDLE_CYCLE,
		&ent->anime_cycle, _enemy_tile_idx
	);	
	
	bool hit_y = ent_move_y(ent, ent->vy);
	// Applies gravity
	if(!hit_y) {
		if(ent->vy < TERMINAL_VY) {
			ent->vy += GRAVITY;
		}
		ent->vx = 0;
	} else {
		if(ent->vx == 0) {
			ent->vx = float2fx(0.75);
		}
		int hit_x = ent_level_collision_at(ent, ent->vx, ent->vy);
		if(!hit_x) {
			ent->vx = -ent->vx;
		}
	}

	ent->vx += -_scroll_x;
	ent_move_x(ent, ent->vx);
	//Take back scroll for next loop
	ent->vx += _scroll_x;

	obj_set_pos(get_ent_att(ent), fx2int(ent->x), fx2int(ent->y));
}
