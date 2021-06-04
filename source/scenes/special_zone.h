#ifndef SPECIAL_ZONE_H
#define SPECIAL_ZONE_H

#include "../common.h"

#define SZ_SHARED_CB 0
#define SZ_GRADIENT_SBB 8

typedef struct sz_data_t
{
	FIXED bg0_x, bg0_y;
	FIXED bg0_dir_x, bg0_dir_y;
	int count;
	BOOL toggle;
} sz_data_t;

extern const scene_t special_zone_scene;

#endif