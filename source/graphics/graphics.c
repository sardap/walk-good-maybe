#include "graphics.h"

#include <tonc.h>
#include <string.h>

#include "backgroundSky.h"
#include "whale.h"

void load_player_tile() {
	// Places the glyphs of a 4bpp boxed metroid sprite 
	//   into LOW obj memory (cbb == 4)
	memcpy(&tile_mem[4][0], whaleTiles, whaleTilesLen);
	memcpy(pal_obj_mem, whalePal, whalePalLen);
}

void init_graphics() {
	REG_BG0CNT= BG_CBB(0) | BG_SBB(30) | BG_4BPP | BG_REG_64x32;
	REG_DISPCNT= DCNT_OBJ | DCNT_OBJ_1D | DCNT_BG0;

	// Load palette
	memcpy(pal_bg_mem, backgroundSkyPal, backgroundSkyPalLen);
	// Load tiles into CBB 0
	memcpy(&tile_mem[0][0], backgroundSkyTiles, backgroundSkyTilesLen);
	// Load map into SBB 30
	memcpy(&se_mem[30][0], backgroundSkyMap, backgroundSkyMapLen);

	load_player_tile();

}