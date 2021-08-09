#ifndef GAME_OVER_H
#define GAME_OVER_H

#include <mm_types.h>

#include "../common.h"

#define GO_SHARED_CB 0
#define GO_MAIN_SBB 22
#define GO_TEXT_SBB 24
#define GO_BLACK_SBB 26

typedef enum go_states_e
{
	GO_STATES_BLACK,
	GO_STATES_FADING_IN,
	GO_STATES_SOLID,
} go_states_e;

typedef struct go_transfer_in_t
{
	int score;
} go_transfer_in_t;

void set_go_in(go_transfer_in_t data);

typedef struct go_data_t
{
	OBJ_ATTR text_objs[128];
	go_states_e state;
	int score;
	int timer;
	int bld_y;
	int volume;
} go_data_t;

extern const scene_t game_over_scene;

#endif