#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <tonc_types.h>

#define SKY_OFFSET 76

#define BUILDINGS_OFFSET 0

// BUILDING 0
#define BUILDING_0_LEFT_ROOF BUILDING_0_RIGHT_ROOF
#define BUILDING_0_LEFT_BOT BUILDING_0_RIGHT_ROOF

#define BUILDING_0_RIGHT_ROOF BUILDINGS_OFFSET
#define BUILDING_0_RIGHT_BOT BUILDING_0_RIGHT_ROOF

#define BUILDING_0_MIDDLE_ROOF BUILDING_0_RIGHT_ROOF
#define BUILDING_0_MIDDLE_BOT BUILDINGS_OFFSET + 1

// BUILDING 1
#define BUILDING_1_OFFSET BUILDINGS_OFFSET + 2

#define BUILDING_1_LEFT_ROOF BUILDING_1_OFFSET
#define BUILDING_1_LEFT_BOT BUILDING_1_OFFSET + 4

#define BUILDING_1_RIGHT_ROOF BUILDING_1_OFFSET + 2
#define BUILDING_1_RIGHT_BOT BUILDING_1_OFFSET + 5

#define BUILDING_1_MIDDLE_ROOF BUILDING_1_OFFSET + 1
#define BUILDING_1_MIDDLE_BOT BUILDING_1_OFFSET + 3

// LAVAL
#define LAVA_OFFSET BUILDING_1_RIGHT_BOT + 1

#define LAVA_LEFT LAVA_OFFSET
#define LAVA_MIDDLE LAVA_LEFT + 1
#define LAVA_RIGHT LAVA_MIDDLE + 1

#define OBJ_TILE_ALLC_SIZE 64
#define BG_TILE_ALLC_SIZE 1024 * 2

void init_graphics();

int allocate_obj_tile_idx(int size);
void free_obj_tile_idx(int idx, int size);

int allocate_bg_tile_idx(int size);
void free_bg_tile_idx(int idx, int size);

int get_buildings_tile_offset();
void set_buildings_tiles_offset(int idx);

bool valid_cloud_address(int tile_offset, int sb, int width, int x, int y);

void create_cloud(int tile_offset, int sb, int width, int x, int y);

void place_n_clouds(int tile_offset, int sb, int width, int n);

#endif