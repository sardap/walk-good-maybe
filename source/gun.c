#include "gun.h"

#include "common.h"
#include "graphics.h"
#include "assets/gun_0_bullet.h"
#include "debug.h"

static int _bullet_top = 0;
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

void create_bullet(bullet_type_t type, FIXED x, FIXED y, FIXED vx, FIXED vy)
{
	write_to_log(LOG_LEVEL_INFO, "CREATING BULLET");

	ent_t *bul = &_ents[_bullet_top++];
	bul->bullet_type = type;
	bul->x = x;
	bul->y = y;
	bul->vx = vx;
	bul->vy = vy;
	bul->active = true;
	bul->ent_type = TYPE_BULLET;

	switch (type)
	{
	case BULLET_TYPE_GUN_0:
		break;
	}

	if (_bullet_top >= ENT_COUNT)
	{
		_bullet_top = 0;
	}

	bul->att_idx = allocate_att(1);
	char str[50];
	sprintf(str, "NB att:%d", bul->att_idx);
	write_to_log(LOG_LEVEL_INFO, str);

	obj_set_attr(&_obj_buffer[bul->att_idx],
				 ATTR0_SQUARE | ATTR0_8BPP, ATTR1_SIZE_8x8,
				 ATTR2_PALBANK(0) | ATTR2_PRIO(0) | ATTR2_ID(_gun_0_tile));

	obj_set_pos(&_obj_buffer[bul->att_idx], fx2int(bul->x), fx2int(bul->y));
}

void update_bullet(ent_t *bul)
{
	bool hit_x = ent_move_x(bul, bul->vx - _scroll_x);
	ent_move_y(bul, bul->vy);

	int col = ent_level_collision_at(bul, 0, 0);

	if (col & (LEVEL_COL_PLAYER))
	{
	}

	if (fx2int(bul->x) > SCREEN_WIDTH || hit_x)
	{
		free_att(1, bul->att_idx);
		bul->ent_type = TYPE_NONE;
		return;
	}

	obj_set_pos(&_obj_buffer[bul->att_idx], fx2int(bul->x), fx2int(bul->y));
}