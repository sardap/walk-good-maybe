#include "enemy.h"

#include <tonc.h>

#include "common.h"
#include "anime.h"
#include "graphics.h"

#include "assets/toast_enemy_idle_01.h"
#include "assets/toast_enemy_idle_02.h"
#include "assets/toast_enemy_idle_03.h"
#include "assets/toast_enemy_idle_04.h"
#include "assets/toast_enemy_idle_05.h"

const uint *enemy_toast_idle_cycle[] = {
	toast_enemy_idle_01Tiles, toast_enemy_idle_01Tiles, toast_enemy_idle_02Tiles, 
	toast_enemy_idle_03Tiles, toast_enemy_idle_04Tiles, toast_enemy_idle_05Tiles
};

static int _enemy_tile_idx;

void load_enemy_toast() {
	_enemy_tile_idx = allocate_tile_idx(2);
	dma3_cpy(&tile_mem[4][_enemy_tile_idx], toast_enemy_idle_01Tiles, toast_enemy_idle_01TilesLen);
}

void create_toast_enemy(ent_t *ent) {
	ent->ent_type = TYPE_ENEMY;

	ent->x = int2fx(50);
	ent->y = int2fx(30);
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

void update_enemy(ent_t *ent) {
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
	}

	obj_set_pos(get_ent_att(ent), fx2int(ent->x), fx2int(ent->y));
}
