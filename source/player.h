#ifndef PLAYER_H
#define PLAYER_H

#include "ent.h"

#define PLAYER_SPAWN_Y 0
#define PLAYER_JUMP_TIME 10
#define PLAYER_LAND_TIME 6
#define PLAYER_LIFE_START 3

#define PLAYER_AIR_CYCLE_COUNT 3
#define PLAYER_WALK_CYCLE_COUNT 5

#define PLAYER_AIR_START_SLOWDOWN ((2.0f * 6) * FIX_SCALEF)
#define PLAYER_AIR_SLOWDOWN_MIN ((1.0f * 6) * FIX_SCALEF)

#define PLAYER_START_JUMP_POWER ((2.0f * 6) * FIX_SCALEF)
#define PLAYER_MAX_JUMP_POWER ((3.0f * 6) * FIX_SCALEF)

#define PLAYER_SHRINKING_TIME 900
#define PLAYER_SHRINK_SIZE 0.75 * FIX_SCALE
#define PLAYER_FULL_SIZE 1 * FIX_SCALE

#define PLAYER_SHRINK_STEP ((PLAYER_SHRINK_SIZE)*FIX_SCALE) / (48 * FIX_SCALE)

#define PLAYER_ADD_JUMP_STEP ((0.1f * 6) * FIX_SCALEF)
#define PLAYER_ADD_SPEED_STEP ((0.2f * 6) * FIX_SCALEF)

#define PLAYER_PRIO (2)

extern ent_t _player;

void init_player();

void load_player_tiles();
void free_player_tiles();

FIXED get_player_jump();
void add_player_jump(FIXED amount);

FIXED get_player_speed();
void add_player_speed(FIXED amount);

void update_player();

int get_player_life();

//This really shouldn't be here
int speed_up_active();

#endif