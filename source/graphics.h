#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <tonc.h>

#define SKY_OFFSET 72

#define BUILDINGS_OFFSET 81

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


void init_graphics();

bool valid_cloud_address(int tile_offset, int sb, int width, int x, int y);

void create_cloud(int tile_offset, int sb, int width, int x, int y);

void place_n_clouds(int tile_offset, int sb, int width, int n);

#endif