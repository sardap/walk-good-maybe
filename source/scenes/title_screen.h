#ifndef TITLE_SCREEN_H
#define TITLE_SCREEN_H

#include "../common.h"

//Background stuff
#define TS_SHARED_CB 0
#define TS_CITY_SSB 24
#define TS_BEACH_SSB 26
#define TS_TEXT_SSB 28
#define TS_OPTION_SSB 30

#define TS_PAL_WATER_START 4
#define TS_PAL_WATER_LENGTH 6

#define TS_PAL_LAVA_START 10
#define TS_PAL_LAVA_LENGTH 3

#define TS_ARROW_Y 115

typedef struct ts_menu_options_t
{
	const scene_t *scene;
	const unsigned short *map;
	const uint mapLen;
	const int lx, rx;
} ts_menu_options_t;

#define TS_OPTIONS_LENGTH 3
extern const ts_menu_options_t _options[TS_OPTIONS_LENGTH];

extern const scene_t title_screen;

#endif