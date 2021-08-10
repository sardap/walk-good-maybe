#ifndef GAME_WIN_H
#define GAME_WIN_H

#include <mm_types.h>

#include "../common.h"

#define GW_SHARED_CB 0
#define GW_MAIN_SBB 22
#define GW_TEXT_SBB 24

typedef enum gw_states_e
{
	GW_STATES_WHITE,
	GW_STATES_FADING_IN,
	GW_STATES_SLIDING,
	GW_STATES_COMPLETE,
} gw_states_e;

typedef struct gw_data_t
{
	gw_states_e state;
	int timer;
	int bld_y;
	FIXED vict_x;
} gw_data_t;

extern const scene_t _game_win_scene;

#endif