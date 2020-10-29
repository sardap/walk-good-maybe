#include "gun.h"

#include "common.h"
#include "graphics.h"
#include "assets/gun_0_bullet.h"
#include "debug.h"

static int _bullet_top = 0;
static int _gun_0_tile = -1;

void load_gun_0_tiles() {
	_gun_0_tile = allocate_tile_idx(1);
	dma3_cpy(&tile_mem[4][_gun_0_tile], gun_0_bulletTiles, gun_0_bulletTilesLen);
}

void unload_gun_0_tiles() {
	free_tile_idx(_gun_0_tile, 1);
}

void create_bullet(bullet_type_t type, FIXED x, FIXED y, FIXED vx, FIXED vy) {
	write_to_log(LOG_LEVEL_INFO, "CREATING BULLET");
	ent_t *bul = &_bullets[_bullet_top++];
	bul->bullet_type = type;
	bul->x = x;
	bul->y = y;
	bul->vx = vx;
	bul->vy = vy;
	bul->active = true;

	switch (type) {
	case BULLET_TYPE_GUN_0:
		break;
	}

	if(_bullet_top >= BULLETS_LENGTH) {
		_bullet_top = 0;
	}

	bul->att_idx = allocate_att(1);
	char str[50];
	sprintf(str, "att:%d", bul->att_idx);
	write_to_log(LOG_LEVEL_INFO, str);

	obj_set_attr(&_obj_buffer[bul->att_idx],
		ATTR0_SQUARE, ATTR1_SIZE_8x8,
		ATTR2_PALBANK(0) | ATTR2_PRIO(0) | ATTR2_ID(_gun_0_tile)
	);

	obj_set_pos(&_obj_buffer[bul->att_idx], fx2int(bul->x), fx2int(bul->y));
}

void update_bullets() {
	for(int i = 0; i < BULLETS_LENGTH; i++) {
		if(!_bullets[i].active) {
			continue;
		}

		ent_move_x(&_bullets[i], _bullets[i].vx - _scroll_x);
		ent_move_y(&_bullets[i], _bullets[i].vy);

		if(fx2int(_bullets[i].x) > SCREEN_WIDTH) {
			free_att(1, _bullets[i].att_idx);
			_bullets[i].active = false;
			continue;
		}

		obj_set_pos(&_obj_buffer[_bullets[i].att_idx], fx2int(_bullets[i].x), fx2int(_bullets[i].y));
	}

}