#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <tonc_types.h>

#define SKY_OFFSET 76

#define OBJ_TILE_ALLC_SIZE 64
#define BG_TILE_ALLC_SIZE 1024 * 2

void init_graphics();

int allocate_obj_tile_idx(int size);
void free_obj_tile_idx(int idx, int size);

int allocate_bg_tile_idx(int size);
void free_bg_tile_idx(int idx, int size);

bool valid_cloud_address(int tile_offset, int sb, int width, int x, int y);

void create_cloud(int tile_offset, int sb, int width, int x, int y);

#endif