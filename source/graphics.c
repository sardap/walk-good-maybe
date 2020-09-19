#include "graphics.h"

#include <tonc.h>
#include <string.h>

#include "common.h"
#include "assets/whale.h"

void load_player_tile() {
	// Places the glyphs of a 4bpp boxed metroid sprite 
	//   into LOW obj memory (cbb == 4)
	dma3_cpy(&tile_mem[4][0], whaleTiles, whaleTilesLen);
	dma3_cpy(pal_obj_mem, whalePal, whalePalLen);
}

void init_graphics() {
	load_player_tile();
}