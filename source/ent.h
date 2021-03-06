#ifndef ENT_H
#define ENT_H

#include "level.h"

#include <tonc_types.h>
#include <tonc_math.h>

#ifdef ISMAIN
#define def
#else
#define def extern
#endif

//Making this above 90 causes linking errors since you run out of iwram
#define ENT_COUNT 25
#define ENT_VISUAL_COUNT 60
#define OBJ_COUNT 128

/*
	These need to be ^2 for col 
	Player colides with bullet and enemy
	00000000 00000000 00000000 00000110

	enemy cloides with player and bullet
	00000000 00000000 00000000 00000101
*/
typedef enum ent_types_t
{
	TYPE_NONE = 0,
	TYPE_PLAYER = (1 << 0),
	TYPE_BULLET = (1 << 2),
	TYPE_ENEMY_BISCUT = (1 << 3),
	TYPE_SPEED_UP = (1 << 4),
	TYPE_ENEMY_BISCUT_UFO = (1 << 5),
	TYPE_ENEMY_BULLET = (1 << 6),
	TYPE_HEALTH_UP = (1 << 7),
	TYPE_JUMP_UP = (1 << 8),
	TYPE_SHRINK_TOKEN = (1 << 9),
	TYPE_SPEICAL_ZONE_PORTAL = (1 << 10),
} ent_types_t;

//These don't need to be bit alligend since we never do cols with them
typedef enum ent_visual_types_t
{
	TYPE_VISUAL_NONE = 0,
	TYPE_VISUAL_SPEED_LINE = 1,
	TYPE_VISUAL_LIFE = 2,
	TYPE_VISUAL_SCORE = 3,
	TYPE_VISUAL_ENEMY_BISUCT_DEATH = 4,
	TYPE_VISUAL_ENEMY_BISUCT_UFO_DEATH = 5,
	TYPE_VISUAL_SPEED_LEVEL = 6,
	TYPE_VISUAL_JUMP_LEVEL = 7,
	TYPE_VISUAL_SPLASH = 8,
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
	int ent_idx;
	FIXED x, y;
	FIXED vx, vy;
	int w, h;
	int ent_cols;
	ent_types_t ent_type;
	//Atrr and affine both have padding in them to make this okay
	union
	{
		OBJ_ATTR att;
		OBJ_AFFINE aff;
	};
	//Ent speifc vars
	union
	{
		// Sepical zone portal
		struct
		{
			int szp_tile_idx;
			int szp_cycle;
		};
		//Bullet
		struct
		{
			bullet_type_t bullet_type;
		};
		//Enemy bisuct
		struct
		{
			int eb_tile_idx;
			int eb_anime_cycle;
		};
		//Enemy bisuct ufo
		struct
		{
			int ebu_tile_id;
			int ebu_anime_cycle;
			int ebu_next_shoot;
		};
	};

} ent_t;

typedef struct visual_ent_t
{
	int ent_idx;
	FIXED x, y;
	ent_visual_types_t type;
	OBJ_ATTR att;
	union
	{
		//Speed line
		struct
		{
			FIXED sl_vx;
		};
		//enemy bisuct death
		struct
		{
			int eb_tile_idx;
			int eb_anime_cycle;
		};
		//Splash
		struct
		{
			int sp_tile_idx;
			int sp_anime_cycle;
		};
	};
} visual_ent_t;

def OBJ_ATTR _obj_buffer[OBJ_COUNT];
def ent_t _player;
def ent_t _ents[ENT_COUNT];
def visual_ent_t _visual_ents[ENT_VISUAL_COUNT];

void init_all_ents();

ent_t *allocate_ent();
void free_ent(ent_t *ent);

visual_ent_t *allocate_visual_ent();
void free_visual_ent(visual_ent_t *ent);

void free_all_ents();

void copy_ents_to_oam();

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

void push_up_from_ground(ent_t *e);

bool did_hit_x(ent_t *e, FIXED dx);
bool ent_move_x(ent_t *e, FIXED dx);
void ent_move_x_dirty(ent_t *e);

bool did_hit_y(ent_t *e, FIXED dy);
bool ent_move_y(ent_t *e, FIXED dy);
void ent_move_y_dirty(ent_t *e);

void update_ents();

void update_visual_ents();

#endif