#include "ent.h"

#include <tonc.h>
#include "common.h"
#include "level.h"

OBJ_ATTR _obj_buffer[128] = {};
FIXED _bg_pos_x = 0;

FIXED translate_x(ent_t *e) {
	return e->x + _bg_pos_x;
}

FIXED translate_y(ent_t *e) {
	return e->y;
}

static inline FIXED level_to_screen(int l) {
	return int2fx(l * TILE_WIDTH + l % TILE_WIDTH);
}

void apply_gravity(ent_t *e) {
	// FIXED level_x = translate_x(e);
	// FIXED level_y = translate_y(e);

	int flags = level_collision_at(e, 0, GRAVITY);
	
	se_mem[30][1] = flags;

	// if(flags & (LEVEL_COL_GROUND)) {
	// 	_player.vy = 0;
	// } else {
	// 	_player.vy += GRAVITY;
	// }
}
