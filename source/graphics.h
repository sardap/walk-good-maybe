#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <tonc_types.h>
#include <tonc_math.h>

#define OBJ_TILE_ALLC_SIZE 127
#define BG_TILE_ALLC_SIZE 1024

void init_graphics();

int allocate_obj_tile_idx(int size);
void free_obj_tile_idx(int idx, int size);

int allocate_bg_tile_idx(int size);
void free_bg_tile_idx(int idx, int size);

inline void cycle_palate(COLOR *target_pal, const int target_pal_start, const u16 *cycle_pal, int *current_idx, const int length)
{
	for (int i = 0; i < length; i++)
	{
		int pal_idx = WRAP(*current_idx + i, 0, length - 1);
		target_pal[target_pal_start + i] = cycle_pal[pal_idx];
	}
	*current_idx = WRAP(*current_idx + 1, 0, length - 1);
}

#endif