#include "numbers.h"

#include <tonc.h>
#include "ent.h"
#include "graphics.h"
#include "assets/numbersfont.h"
#include "assets/spriteShared.h"
#include "debug.h"
#include "common.h"

static int _tile_start_idx;
static int _number;

void load_number_tiles()
{
	_tile_start_idx = allocate_obj_tile_idx(10);
	dma3_cpy(&tile_mem[4][_tile_start_idx], numbersfontTiles, numbersfontTilesLen);
	dma3_cpy(pal_obj_mem, spriteSharedPal, spriteSharedPalLen);

	_number = 0;
}

void free_number_tiles()
{
	free_obj_tile_idx(_tile_start_idx, 10);
}

int get_number_tile_start()
{
	return _tile_start_idx;
}