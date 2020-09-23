#ifndef GRAPHICS_H
#include <tonc.h>

#define BUILDINGS_OFFSET 81

#define BUILDING_0_ROOF_RIGHT BUILDINGS_OFFSET
#define BUILDING_0_MIDDLE_RIGHT BUILDING_0_ROOF_RIGHT

#define BUILDING_0_ROOF_LEFT BUILDING_0_ROOF_RIGHT
#define BUILDING_0_LEFT_BOT BUILDING_0_ROOF_RIGHT

#define BUILDING_0_MIDDLE_ROOF BUILDING_0_ROOF_RIGHT
#define BUILDING_0_MIDDLE_BOT BUILDINGS_OFFSET + 1

void init_graphics();

void load_player_tile();

bool valid_cloud_address(int tile_offset, int sb, int width, int x, int y);

void create_cloud(int tile_offset, int sb, int width, int x, int y);

void place_n_clouds(int tile_offset, int sb, int width, int n);

#endif