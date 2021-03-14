#ifndef GAME_INTRO_SCREEN_H
#define GAME_INTRO_SCREEN_H

#include "../common.h"

typedef enum gi_states_t
{
	GI_S_READY,
	GI_S_SET,
	GI_S_GO,
} gi_states_t;

extern const scene_t game_intro;

#endif