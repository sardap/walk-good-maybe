#ifndef SCENE_SHARED_H
#define SCENE_SHARED_H

#include "main_game.h"
#include "game_intro.h"

typedef union shared_data_t
{
	mg_data_t mg;
	gi_data_t gi;
} shared_data_t;

extern shared_data_t _shared_data;

#endif