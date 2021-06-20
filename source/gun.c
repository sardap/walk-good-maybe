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

void free_gun_0_tiles()
{
	free_obj_tile_idx(_gun_0_tile, 1);
}

int gun_0_tiles()
{
	return _gun_0_tile;
}

void create_bullet(
	ent_t *bul, bullet_type_t type,
	FIXED x, FIXED y, FIXED vx, FIXED vy, int flip)
{
	bul->ent_type = TYPE_BULLET;

	bul->x = x;
	bul->w = 5;
	bul->y = y;
	bul->h = 5;
	bul->vx = vx;
	bul->vy = vy;

	bul->bullet_type = type;

	switch (type)
	{
	case BULLET_TYPE_GUN_0:
		break;
	}

	bul->att.attr0 = ATTR0_SQUARE | ATTR0_8BPP;
	bul->att.attr1 = ATTR1_SIZE_8x8 || flip ? ATTR1_HFLIP : 0;
	bul->att.attr2 = ATTR2_ID(_gun_0_tile);
}

void update_bullet(ent_t *ent)
{
	bool hit_x = ent_move_x(ent, ent->vx - _scroll_x);
	ent_move_y(ent, ent->vy);

	if (
		fx2int(ent->x) > SCREEN_WIDTH ||
		fx2int(ent->x) < 0 ||
		hit_x ||
		ent->ent_cols & (TYPE_ENEMY_BISCUT | TYPE_ENEMY_BISCUT_UFO))
	{
		free_ent(ent);
		ent->ent_type = TYPE_NONE;
		return;
	}

	obj_set_pos(&ent->att, fx2int(ent->x), fx2int(ent->y));
}