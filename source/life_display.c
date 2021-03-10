#include "life_display.h"

#include "ent.h"
#include "common.h"
#include "graphics.h"
#include "player.h"

#include "assets/lifeAmmount3.h"
#include "assets/lifeAmmount2.h"
#include "assets/lifeAmmount1.h"

static int _life_tile_start_idx;
static int _att_life_idx;

void load_life_display()
{
	_life_tile_start_idx = allocate_obj_tile_idx(4);
	update_life_display(PLAYER_LIFE_START);

	_att_life_idx = allocate_att(1);
	obj_set_attr(
		&_obj_buffer[_att_life_idx],
		ATTR0_SQUARE | ATTR0_8BPP, ATTR1_SIZE_16x16,
		ATTR2_PALBANK(0) | ATTR2_PRIO(0) | ATTR2_ID(_life_tile_start_idx));
	obj_set_pos(&_obj_buffer[_att_life_idx], GBA_WIDTH - 16, 0);
}

void update_life_display(int life)
{
	u32 *tiles = (u32 *)lifeAmmount1Tiles;
	switch (life)
	{
	case 3:
		tiles = (u32 *)lifeAmmount3Tiles;
		break;
	case 2:
		tiles = (u32 *)lifeAmmount2Tiles;
		break;
	}
	dma3_cpy(&tile_mem[4][_life_tile_start_idx], tiles, lifeAmmount3TilesLen);
}

void unload_life_display()
{
	free_obj_tile_idx(_life_tile_start_idx, 4);
	free_att(_att_life_idx, 1);
}
