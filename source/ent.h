#ifndef ENT_H
#define ENT_H

#include "level.h"

#include <tonc_types.h>

#ifdef ISMAIN
#define def
#else
#define def extern
#endif

//Making this above 90 causes linking errors (maybe running out of ram? that's not how ram works?)
#define ENT_COUNT 25
#define ENT_VISUAL_COUNT 5

typedef enum ent_types_t
{
	TYPE_NONE = 0,
	TYPE_PLAYER = 1,
	TYPE_BULLET = 2,
	TYPE_ENEMY = 4,
	TYPE_SPEED_UP = 8,
	TYPE_SPEED_LINE = 16,
} ent_types_t;

typedef enum ent_visual_types_t
{
	TYPE_VISUAL_NONE = 0,
	TYPE_VISUAL_SPEED_LINE = 1,
} ent_visual_types_t;

typedef enum movement_state_t
{
	MOVEMENT_GROUNDED,
	MOVEMENT_JUMPING,
	MOVEMENT_AIR,
	MOVEMENT_LANDED
} movement_state_t;

typedef enum facing_t
{
	FACING_LEFT,
	FACING_RIGHT
} facing_t;

typedef enum
{
	BULLET_TYPE_GUN_0
} bullet_type_t;

typedef struct ent_t
{
	FIXED x, y;
	FIXED vx, vy;
	int tid;
	int w, h;
	int att_idx;
	int ent_cols;
	ent_types_t ent_type;
	
	//Ent speifc vars
	union
	{
		//Player
		struct
		{
			movement_state_t move_state;
			facing_t facing;
			FIXED jump_power;
		};
		//Bullet
		struct
		{
			bullet_type_t bullet_type;
			bool active;
		};
		//Enemy
		struct
		{
		};
	};

} ent_t;

typedef struct ent_visual_t
{
	FIXED x, y;
	FIXED vx, vy;
	ent_visual_types_t type;
} ent_visual_t;

def OBJ_ATTR _obj_buffer[128];
def ent_t _player;
def ent_t _ents[ENT_COUNT];

void init_obj_atts();
int allocate_ent(int count);
void free_ent(int idx, int count);

int allocate_ent_visual(int count);
void free_ent_visual(int idx, int count);

inline OBJ_ATTR *get_ent_att(ent_t *e)
{
	return &_obj_buffer[e->att_idx];
}

FIXED translate_x(ent_t *e);
FIXED translate_y(ent_t *e);

inline int ent_level_collision(ent_t *e)
{
	return level_collision_rect(
		translate_x(e) / FIX_SCALE,
		translate_y(e) / FIX_SCALE,
		e->w,
		e->h);
}

inline int ent_level_collision_at(ent_t *e, FIXED vx, FIXED vy)
{
	return level_collision_rect(
		(translate_x(e) + vx) / FIX_SCALE,
		(translate_y(e) + vy) / FIX_SCALE,
		e->w,
		e->h);
}

int att_count();

void push_up_from_ground(ent_t *e);

bool did_hit_x(ent_t *e, FIXED dx);
bool ent_move_x(ent_t *e, FIXED dx);
void ent_move_x_dirty(ent_t *e);

bool did_hit_y(ent_t *e, FIXED dy);
bool ent_move_y(ent_t *e, FIXED dy);
void ent_move_y_dirty(ent_t *e);

bool apply_gravity(ent_t *e);

void update_ents();

#endif