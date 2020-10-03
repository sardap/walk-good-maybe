#ifndef ENT_H
#define ENT_H

#include "level.h"

#include <tonc_types.h>

#ifdef ISMAIN
	#define def
#else
	#define def extern
#endif

typedef enum movement_state_t {
	MOVEMENT_GROUNDED, MOVEMENT_AIR,
} movement_state_t;

typedef enum facing_t {
	FACING_LEFT, FACING_RIGHT
} facing_t;

typedef struct ent_t {
	FIXED x, y;
	FIXED vx, vy;
	int tid;
	int w, h;

	union  {
		struct {
			movement_state_t move_state;
			facing_t facing;
		};
	};
	
} ent_t;

def OBJ_ATTR _obj_buffer[128];
def ent_t _player;

FIXED translate_x(ent_t *e);
FIXED translate_y(ent_t *e);

inline int ent_level_collision(ent_t *e) {
	return level_collision_rect(
		translate_x(e) / FIX_SCALE,
		translate_y(e) / FIX_SCALE,
		e->w,
		e->h
	);
}

inline int ent_level_collision_at(ent_t *e, FIXED vx, FIXED vy) {
	return level_collision_rect(
		(translate_x(e) + vx) / FIX_SCALE, 
		(translate_y(e) + vy) / FIX_SCALE,
		e->w, 
		e->h
	);
}

void push_up_from_ground(ent_t *e);

bool did_hit_x(ent_t *e, FIXED dx);
bool ent_move_x(ent_t *e, FIXED dx);

bool did_hit_y(ent_t *e, FIXED dy);
bool ent_move_y(ent_t *e, FIXED dy);

bool apply_gravity(ent_t *e);

#endif