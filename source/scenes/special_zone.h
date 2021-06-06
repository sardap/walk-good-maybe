#ifndef SPECIAL_ZONE_H
#define SPECIAL_ZONE_H

#include "../common.h"

#define SZ_SHARED_CB 0
#define SZ_GRID_SBB 24
#define SZ_EYE_SBB (SZ_GRID_SBB + 2)
#define SZ_TEXT_SBB (SZ_EYE_SBB + 2)

#define SZ_GRID_LAYER 3
#define SZ_EYE_LAYER (SZ_GRID_LAYER - 1)
#define SZ_OBJECT_LAYER (SZ_EYE_LAYER - 1)
#define SZ_TEXT_LAYER (SZ_OBJECT_LAYER - 1)

#define SZ_PLAYER_WIDTH_FX (32 * FIX_SCALE)
#define SZ_PLAYER_HEIGHT_FX SZ_PLAYER_WIDTH_FX
#define SZ_PLAYER_VELOCITY (0.025f * FIX_SCALEF)

#define SZ_OBS_COUNT 15
#define SZ_OBS_WIDTH_FX (8 * FIX_SCALE)
#define SZ_OBS_HEIGHT_FX (8 * FIX_SCALE)

typedef enum sz_text_state_e
{
	SZ_TS_FADING,
	SZ_TS_FADED,
	SZ_TS_UNFDAING,
	SZ_TS_SOLID
} sz_text_state_e;

typedef struct sz_player_t
{
	OBJ_ATTR *attr;
	OBJ_AFFINE *aff;
	FIXED x, y;
	int angle;
	FIXED turning_speed;
	FIXED max_velocity;
	FIXED velocity;
} sz_player_t;

typedef enum sz_obs_e
{
	SZ_OBS_COIN,
	SZ_OBS_POSION
} sz_obs_e;

typedef struct sz_obs_t
{
	OBJ_ATTR *attr;
	FIXED x, y, dx, dy;
	BOOL enabled;
} sz_obs_t;

typedef struct sz_data_t
{
	OBJ_AFFINE *obj_aff_buffer;
	FIXED bg0_x, bg0_y;
	FIXED bg0_dir_x, bg0_dir_y;
	int grid_count, eye_count, text_fade_count;
	int text_eva;
	sz_text_state_e text_state;
	BOOL grid_toggle;
	BOOL eye_toggle;
	sz_player_t player;
	sz_obs_t obs[SZ_OBS_COUNT];
} sz_data_t;

extern const scene_t special_zone_scene;

#endif