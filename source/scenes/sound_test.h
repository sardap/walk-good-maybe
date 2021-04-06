#ifndef SOUND_TEST_H
#define SOUND_TEST_H

#include "../common.h"

#define ST_SHARED_CB 0
#define ST_COLOUR_SBB 24

#define ST_OBJ_COUNT 90

typedef enum st_slection_t
{
	ST_SELECTION_FX,
	ST_SELECTION_MUSIC,
	ST_SELECTION_COUNT,
} st_slection_t;

typedef struct st_data_t
{
	OBJ_ATTR objs[ST_OBJ_COUNT];
	OBJ_ATTR *sound_fx_left_arrow, *sound_fx_right_arrow;
	int fx_idx;
	OBJ_ATTR *sound_mus_left_arrow, *sound_mus_right_arrow;
	int mus_idx;
	OBJ_ATTR *selection_arrow;
	int arrow_countdown;
	st_slection_t fx_mode;
} st_data_t;

extern const scene_t sound_test_scene;

#endif