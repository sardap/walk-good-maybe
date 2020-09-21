#ifndef ENT_H

#include <tonc.h>

#ifdef ISMAIN
	#define def
#else
	#define def extern
#endif

typedef enum facing_t {
	FACING_LEFT, FACING_RIGHT
} facing_t;

typedef struct ent_t {
	int x, y;
	int vx, vy;
	int tid;

	union  {
		struct {
			facing_t facing;
		};
	};
	
} ent_t;

def OBJ_ATTR _obj_buffer[128];
def OBJ_ATTR *_player_obj;
def ent_t _player;

#endif