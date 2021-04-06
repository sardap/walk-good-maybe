#ifndef GEN_H
#define GEN_H

#define BUILDING_Y_SPAWN 136
#define CITY_BUILDING_Y_TILE_SPAWN (BUILDING_Y_SPAWN / 8)

#define BEACH_Y_SPAWN 144
#define BEACH_ISLAND_Y_TILE_SPAWN (BEACH_Y_SPAWN / 8)

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

// BUILDING 3
#define BUILDING_3_OFFSET 0

#define BUILDING_3_LEFT BUILDING_3_OFFSET
#define BUILDING_3_RIGHT BUILDING_3_OFFSET + 5

#define BUILDING_3_MIDDLE_WHITE BUILDING_3_OFFSET + 6
#define BUILDING_3_MIDDLE_WINDOW BUILDING_3_OFFSET + 7

#define BUILDING_3_ROOF_TOP BUILDING_3_OFFSET + 1
#define BUILDING_3_ROOF_LEFT BUILDING_3_OFFSET + 2
#define BUILDING_3_ROOF_MIDDLE BUILDING_3_OFFSET + 3
#define BUILDING_3_ROOF_RIGHT BUILDING_3_OFFSET + 4

// BUILDING 4
#define BUILDING_4_OFFSET 0

#define BUILDING_4_LEFT_BOT BUILDING_4_OFFSET
#define BUILDING_4_MIDDLE_BLANK BUILDING_4_OFFSET + 2
#define BUILDING_4_RIGHT_BOT BUILDING_4_OFFSET + 1
#define BUILDING_4_MIDDLE_WINDOW BUILDING_4_OFFSET + 3

#define BUILDING_4_SIGN_YELLOW_TOP BUILDING_4_OFFSET + 4
#define BUILDING_4_SIGN_YELLOW_BOT BUILDING_4_OFFSET + 5

#define BUILDING_4_SIGN_ORANGE_TOP BUILDING_4_OFFSET + 6
#define BUILDING_4_SIGN_ORANGE_BOT BUILDING_4_OFFSET + 7

#define BUILDING_4_SIGN_GREEN_TOP BUILDING_4_OFFSET + 8
#define BUILDING_4_SIGN_GREEN_BOT BUILDING_4_OFFSET + 9

#define BUILDING_4_SIGN_BLUE BUILDING_4_OFFSET + 10
#define BUILDING_4_SIGN_RED BUILDING_4_OFFSET + 11

#define BUILDING_4_ROOF_LEFT BUILDING_4_OFFSET + 12
#define BUILDING_4_ROOF_MIDDLE BUILDING_4_OFFSET + 13
#define BUILDING_4_ROOF_RIGHT BUILDING_4_OFFSET + 14

// BUILDING 5
#define BUILDING_5_OFFSET 0

#define BUILDING_5_ROOF_LEFT BUILDING_5_OFFSET
#define BUILDING_5_ROOF_RIGHT BUILDING_5_OFFSET + 1
#define BUILDING_5_ROOF_MIDDLE BUILDING_5_OFFSET + 2

#define BUILDING_5_MIDDLE_LEFT BUILDING_5_OFFSET + 3
#define BUILDING_5_MIDDLE_RIGHT BUILDING_5_OFFSET + 4

#define BUILDING_5_MIDDLE_WINDOW BUILDING_5_OFFSET + 5

// BUILDING 5
#define BUILDING_6_OFFSET 0

#define BUILDING_6_BLANK BUILDING_6_OFFSET
#define BUILDING_6_ANGLE_LEFT BUILDING_6_OFFSET + 1

// LAVA
#define LAVA_OFFSET 0

#define LAVA_TILE_LEFT LAVA_OFFSET
#define LAVA_TILE_MIDDLE LAVA_OFFSET + 1
#define LAVA_TILE_RIGHT LAVA_OFFSET + 2
#define LAVA_TILE_END LAVA_TILE_RIGHT

/**
 *
 * Islands
 * 
 */

//Island 1
#define ISLAND_0_OFFSET 0

#define ISLAND_0_BOTTOM (ISLAND_0_OFFSET + 0)
#define ISLAND_0_MIDDLE (ISLAND_0_OFFSET + 1)
#define ISLAND_0_TOP (ISLAND_0_OFFSET + 2)

typedef struct t_spawn_info
{
	int enemy_chance;
	int lava_chance;
	int token_chance;
} t_spawn_info;

void init_gen();

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

void load_building_3(int cb);
void unload_building_3();
int spawn_building_3(int start_x);

void load_building_4(int cb);
void unload_building_4();
int spawn_building_4(int start_x);

void load_building_5(int cb);
void free_building_5();
int spawn_building_5(int start_x);

void load_building_6(int cb);
void free_building_6();
int spawn_building_6(int start_x);

void load_island_00(int cb);
void free_island_00();
int spawn_island_00(int start_x);

#endif