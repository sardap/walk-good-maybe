#ifndef TITLE_SCREEN_H
#define TITLE_SCREEN_H

#include "../common.h"

//Background stuff
#define TS_SHARED_CB 0
#define TS_CITY_SSB 24
#define TS_BEACH_SSB 26
#define TS_TEXT_SSB 28
#define TS_OPTION_SSB 30

#define TS_PAL_LAVA_START 10
#define TS_PAL_LAVA_LENGTH 3

#define TS_ARROW_Y 115

typedef enum ts_menu_enum
{
	TS_MENU_CITY,
	TS_MENU_BEACH,
	TS_MENU_CREDITS,
	TS_MENU_SOUND_TEST
} ts_menu_enum;

typedef struct ts_menu_options_t
{
	ts_menu_enum type;
	const unsigned short *map;
	const uint mapLen;
	const int lx, rx;
	//Bad but cbf
} ts_menu_options_t;

extern const scene_t title_screen;

#endif