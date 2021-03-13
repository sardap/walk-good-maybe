#ifndef MAIN_GAME_H
#define MAIN_GAME_H

#define MG_SHARED_CB 0
#define MG_TEXT_SB 22
#define MG_CITY_SB 30
#define MG_CLOUD_SB 24
#define MG_BUILDING_SB 26

#define CLOUD_WIDTH (int)((4 * 8) * (FIX_SCALE))

#define MG_BG_X 64
#define MG_BG_X_PIX (int)(MG_BG_X * 8 * (FIX_SCALE))

typedef enum mg_mode_t
{
	MG_MODE_CITY,
	MG_MODE_BEACH,
} mg_mode_t;

typedef enum mg_states_t
{
	MG_S_READY,
	MG_S_SET,
	MG_S_GO,
	MG_S_STARTING,
	MG_S_SCROLLING,
	MG_S_PAUSED
} mg_states_t;

#endif