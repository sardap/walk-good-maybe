#ifndef SCENE_SHARED_H
#define SCENE_SHARED_H

#include "main_game.h"
#include "game_intro.h"
#include "credits.h"

typedef union shared_data_t
{
	mg_data_t mg;
	gi_data_t gi;
	cr_data_t cr;
} shared_data_t;

extern shared_data_t _shared_data;

#endif