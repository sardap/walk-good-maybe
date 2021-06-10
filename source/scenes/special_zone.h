#ifndef SPECIAL_ZONE_H
#define SPECIAL_ZONE_H

#include "../common.h"

#define SZ_SHARED_CB 0
#define SZ_GRID_SBB 24
#define SZ_EYE_SBB (SZ_GRID_SBB + 2)
#define SZ_TEXT_SBB (SZ_EYE_SBB + 2)
#define SZ_UI_SBB (SZ_TEXT_SBB + 2)

#define SZ_GRID_LAYER 3
#define SZ_EYE_LAYER (SZ_GRID_LAYER - 1)
#define SZ_OBJECT_LAYER (SZ_EYE_LAYER - 1)
#define SZ_TEXT_LAYER (SZ_OBJECT_LAYER - 1)
#define SZ_UI_LAYER SZ_TEXT_LAYER

#define SZ_PLAYER_WIDTH_FX (30 * FIX_SCALE)
#define SZ_PLAYER_HEIGHT_FX SZ_PLAYER_WIDTH_FX
#define SZ_PLAYER_VELOCITY (0.01f * FIX_SCALEF)

#define SZ_OBS_COUNT 20
#define SZ_OBS_WIDTH_FX (8 * FIX_SCALE)
#define SZ_OBS_HEIGHT_FX (8 * FIX_SCALE)

#define SZ_SCORE_DIGIT_COUNT (2)

#define SZ_BORDER_PAL_IDX 4

#define SZ_MOTUH_OPEN_TIME 72

typedef enum sz_text_state_e
{
	SZ_TS_SOLID,
	SZ_TS_EYES_OPEN,
	SZ_TS_FADING,
	SZ_TS_FADED,
	SZ_TS_UNFDAING
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
	int good_collected;
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

typedef struct sz_ui_t
{
	int score_number_offset;
} sz_ui_t;

typedef struct sz_data_t
{
	OBJ_AFFINE *obj_aff_buffer;
	FIXED bg0_x, bg0_y;
	FIXED bg0_dir_x, bg0_dir_y;
	int grid_count, text_fade_count;
	int text_eva;
	sz_text_state_e text_state;
	BOOL grid_toggle;
	BOOL eyes_looking;
	int mouth_open_countdown;
	sz_ui_t ui;
	sz_player_t player;
	sz_obs_t obs[SZ_OBS_COUNT];
	int obj_count;
	FIXED colour_dist;
	u16 border_colour_current;
	u16 border_colour_next;
} sz_data_t;

extern const scene_t special_zone_scene;

#endif