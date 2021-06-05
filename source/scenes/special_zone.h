#ifndef SPECIAL_ZONE_H
#define SPECIAL_ZONE_H

#include "../common.h"

#define SZ_SHARED_CB 0
#define SZ_GRID_SBB 24
#define SZ_EYE_SBB (SZ_GRID_SBB + 2)
#define SZ_TEXT_SBB (SZ_EYE_SBB + 2)

#define SZ_GRID_LAYER 2
#define SZ_EYE_LAYER (SZ_GRID_LAYER - 1)
#define SZ_TEXT_LAYER (SZ_EYE_LAYER - 1)

typedef struct sz_data_t
{
	FIXED bg0_x, bg0_y;
	FIXED bg0_dir_x, bg0_dir_y;
	int grid_count, eye_count, text_fade_count;
	int text_eva;
	BOOL text_increment;
	BOOL grid_toggle;
	BOOL eye_toggle;
} sz_data_t;

extern const scene_t special_zone_scene;

#endif