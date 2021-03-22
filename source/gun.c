#include "gun.h"

#include "common.h"
#include "graphics.h"
#include "assets/gun_0_bullet.h"
#include "debug.h"

static int _gun_0_tile = -1;

void load_gun_0_tiles()
{
	_gun_0_tile = allocate_obj_tile_idx(1);
	dma3_cpy(&tile_mem[4][_gun_0_tile], gun_0_bulletTiles, gun_0_bulletTilesLen);
}

void unload_gun_0_tiles()
{
	free_obj_tile_idx(_gun_0_tile, 1);
}

int gun_0_tiles()
{
	return _gun_0_tile;
}

void create_bullet(
	ent_t *bul, int att_idx, bullet_type_t type,
	FIXED x, FIXED y, FIXED vx, FIXED vy, int flip)
{
	bul->ent_idx = att_idx;
	bul->bullet_type = type;
	bul->x = x;
	bul->w = 5;
	bul->y = y;
	bul->h = 5;
	bul->vx = vx;
	bul->vy = vy;
	bul->ent_type = TYPE_BULLET;

	switch (type)
	{
	case BULLET_TYPE_GUN_0:
		break;
	}

	obj_set_attr(
		get_ent_att(bul),
		ATTR0_SQUARE | ATTR0_8BPP, ATTR1_SIZE_8x8 || flip ? ATTR1_HFLIP : 0,
		ATTR2_ID(_gun_0_tile));

	obj_set_pos(get_ent_att(bul), fx2int(bul->x), fx2int(bul->y));
}

void update_bullet(ent_t *bul)
{
	bool hit_x = ent_move_x(bul, bul->vx - _scroll_x);
	ent_move_y(bul, bul->vy);

	if (fx2int(bul->x) > SCREEN_WIDTH || fx2int(bul->x) < 0 || hit_x || bul->ent_cols & (TYPE_ENEMY))
	{
		free_ent(bul->ent_idx, 1);
		bul->ent_type = TYPE_NONE;
		return;
	}

	obj_set_pos(get_ent_att(bul), fx2int(bul->x), fx2int(bul->y));
}