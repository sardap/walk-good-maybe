#ifndef MAIN_GAME_H
#define MAIN_GAME_H

#include "../common.h"

#define MG_SHARED_CB 0
#define MG_PAUSE_SBB 22
#define MG_FAR_SB 30
#define MG_CLOUD_SB 24
#define MG_PLATFROM_SB 26

#define CLOUD_WIDTH (int)((4 * 8) * (FIX_SCALE))

#define MG_BG_X 64
#define MG_BG_X_PIX (int)(MG_BG_X * 8 * (FIX_SCALE))

#define MG_MAX_SCROLL_SPEED (int)(10 * FIX_SCALE)

typedef enum mg_mode_t
{
	MG_MODE_CITY,
	MG_MODE_BEACH,
} mg_mode_t;

typedef enum mg_states_t
{
	MG_S_STARTING,
	MG_S_SCROLLING,
	MG_S_PAUSED
} mg_states_t;

typedef struct mg_data_t
{
	FIXED next_building_spawn;
	int building_spawn_x;
	int bg_0_scroll;
	int bg_2_scroll;

	int water_pal_idx;
	
	int far_tiles_idx;

	mg_states_t state;
	mg_states_t old_state;
	mg_mode_t mode;
} mg_data_t;

typedef struct mg_parm_t
{
	mg_mode_t mode;
} mg_parm_t;

void setMgParmters(mg_parm_t parm);

extern const scene_t main_game;

#endif