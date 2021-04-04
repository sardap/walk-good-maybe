#ifndef CREDITS_H
#define CREDITS_H

#include <tonc.h>

#include "../common.h"

//Background stuff
#define CR_SHARED_CB 0
#define CR_SPACE_SSB 24
#define CR_IMAGE_SBB 26
#define CR_TEXT_SBB 28

#define CR_TEXT_SCROLL_SPEED (0.5 * FIX_SCALE)
#define CR_FADE_TIME 80
#define CR_PAUSE_TIME 120

extern const scene_t credits_screen;

typedef struct cr_credit_t
{
	const char *str;
	const unsigned short *pal;
	const unsigned int *tiles;
	const unsigned short *map;
} cr_credit_t;

typedef struct cr_active_credit_t
{
	const cr_credit_t *crd;
	FIXED x, y;
} cr_active_credit_t;

typedef enum cr_states_t
{
	CR_STATE_SCROLLING_UP,
	CR_STATE_PAUSE,
	CR_STATE_LEAVE
} cr_states_t;

typedef struct cr_data_t
{
	OBJ_ATTR text_objs[128];
	int active_idx;
	cr_active_credit_t active;
	cr_states_t state;
	int countdown;
	FIXED eva, evb;
} cr_data_t;

#endif