#ifndef GEN_H
#define GEN_H

#define BUILDING_Y_SPAWN 136
#define CITY_BUILDING_Y_TILE_SPAWN BUILDING_Y_SPAWN / 8
#define MAX_JUMP_WIDTH_TILES 7
#define MIN_JUMP_WIDTH_TILES 4
// BUILDING 0
#define BUILDING_0_BRICK 0
#define BUILDING_0_WINDOW BUILDING_0_BRICK + 1

// BUILDING 1
#define BUILDING_1_OFFSET 0

#define BUILDING_1_LEFT_ROOF BUILDING_1_OFFSET
#define BUILDING_1_LEFT_BOT BUILDING_1_OFFSET + 4

#define BUILDING_1_RIGHT_ROOF BUILDING_1_OFFSET + 2
#define BUILDING_1_RIGHT_BOT BUILDING_1_OFFSET + 5

#define BUILDING_1_MIDDLE_ROOF BUILDING_1_OFFSET + 1
#define BUILDING_1_MIDDLE_BOT BUILDING_1_OFFSET + 3

// BUILDING 2
#define BUILDING_2_OFFSET 0

#define BUILDING_2_WINDOW BUILDING_2_OFFSET

#define BUILDING_2_MIDDLE_ROOF_HANGING BUILDING_2_OFFSET + 1
#define BUILDING_2_MIDDLE_ROOF_CLEAN BUILDING_2_OFFSET + 2

#define BUILDING_2_MIDDLE_WHITE BUILDING_2_OFFSET + 3
#define BUILDING_2_MIDDLE_WHITE_CLEAN BUILDING_2_OFFSET + 4

#define BUILDING_2_MIDDLE_BLUE BUILDING_2_OFFSET + 9

#define BUILDING_2_LEFT_MIDDLE BUILDING_2_OFFSET + 5
#define BUILDING_2_RIGHT_MIDDLE BUILDING_2_OFFSET + 6

#define BUILDING_2_LEFT_ROOF BUILDING_2_OFFSET + 7
#define BUILDING_2_RIGHT_ROOF BUILDING_2_OFFSET + 8

// LAVA
#define LAVA_OFFSET 0

#define LAVA_TILE_LEFT LAVA_OFFSET
#define LAVA_TILE_MIDDLE LAVA_OFFSET + 1
#define LAVA_TILE_RIGHT LAVA_OFFSET + 2
#define LAVA_TILE_END LAVA_TILE_RIGHT

int get_lava_tile_offset();
int get_buildings_tile_offset();
int get_buildings_tile_offset_end();

void load_lava_0(int cb);
void unload_lava_0();

void load_building_0(int cb);
void unload_building_0();
int spawn_building_0(int start_x);

void load_building_1(int cb);
void unload_building_1();
int spawn_building_1(int start_x);

void load_building_2(int cb);
void unload_building_2();
int spawn_building_2(int start_x);

#endif