#include <tonc.h>
#include "common.h"
#include "graphics.h"

typedef enum facing_t {
	FACING_LEFT, FACING_RIGHT
} facing_t;

typedef struct player_t {
	int x;
	int y;
	int tid;
	int pb;
	facing_t facing;
} player_t;

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer= (OBJ_AFFINE*)obj_buffer;

player_t player;

void sprite_init() {
	init_graphics();
	oam_init(obj_buffer, 128);
}

void init_player() {
	player.x = (240/2)-32;
	player.y = 130;
	player.tid = 0;
	player.pb = 0;
	player.facing = FACING_RIGHT;
}

void game_loop() {

	// OBJ_ATTR *metr= &obj_buffer[0];

	// obj_set_attr(metr, 
	// 	ATTR0_SQUARE, ATTR1_SIZE_32,
	// 	ATTR2_PALBANK(player.pb) | player.tid
	// );

	// // position sprite (redundant here; the _real_ position
	// // is set further down
	// obj_set_pos(metr, player.x, player.y);

	while(1)
	{
		vid_vsync();
		key_poll();

		common_step();

		scene_update();

		continue;

		// move left/right
		// player.x += 2*key_tri_horz();
		// if(player.facing == FACING_RIGHT && key_hit(KEY_LEFT)) {
		// 	player.facing = FACING_LEFT;
		// 	metr->attr1 ^= ATTR1_HFLIP;
		// } else if(player.facing == FACING_LEFT && key_hit(KEY_RIGHT)) {
		// 	player.facing = FACING_RIGHT;
		// 	metr->attr1 ^= ATTR1_HFLIP;
		// }

		// move up/down
		player.y += 2*key_tri_vert();

		// increment/decrement starting tile with R/L
		player.tid += bit_tribool(key_hit(KEY_START), KI_R, KI_L);

		// make it glow (via palette swapping)
		player.pb = key_is_down(KEY_SELECT) ? 1 : 0;

		// toggle mapping mode
		if(key_hit(KEY_START))
			REG_DISPCNT ^= DCNT_OBJ_1D;

		// Hey look, it's one of them build macros!
		// metr->attr2 = ATTR2_BUILD(player.tid, player.pb, 0);
		// obj_set_pos(metr, player.x, player.y);

		oam_copy(oam_mem, obj_buffer, 1);	// only need to update one
	}
}

void start_game() {
	init_player();

	int count = 4091950981;
	while (1) {
		vid_vsync();
		key_poll();

		count *= 4259141111;

		if(key_hit(KEY_A)) {
			break;
		}
	}
	
	init_seed(count);

	scene_set(main_game);

	game_loop();
}

int main()
{
	init_graphics();

	start_game();

    while(1);

    return 0;
}
