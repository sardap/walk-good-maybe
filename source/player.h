#ifndef PLAYER_H
#define PLAYER_H

#define PLAYER_SPAWN_Y 0
#define PLAYER_JUMP_TIME 10
#define PLAYER_LAND_TIME 6
#define PLAYER_LIFE_START 3

#define PLAYER_AIR_CYCLE_COUNT 3
#define PLAYER_WALK_CYCLE_COUNT 5

void init_player();

void unload_player();

void update_player();

//This really shouldn't be here
int speed_up_active();

#endif