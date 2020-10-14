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
	MOVEMENT_GROUNDED, MOVEMENT_JUMPING, MOVEMENT_AIR,
} movement_state_t;

typedef enum facing_t {
	FACING_LEFT, FACING_RIGHT
} facing_t;

typedef struct ent_t {
	FIXED x, y;
	FIXED vx, vy;
	int tid;
	int w, h;
	int att_idx;

	union  {
		struct {
			movement_state_t move_state;
			facing_t facing;
			FIXED jump_power;
		};
	};
	
} ent_t;

def OBJ_ATTR _obj_buffer[128];
def ent_t _player;

void init_obj_atts();
int allocate_att(int count);
void free_att(int count, int idx);

inline OBJ_ATTR* get_ent_att(ent_t *e) {
	return &_obj_buffer[e->att_idx];
}

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