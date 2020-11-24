#ifndef PLAYER_H
#define PLAYER_H

#define PLAYER_SPAWN_Y 0
#define PLAYER_JUMP_TIME 10
#define PLAYER_LAND_TIME 6
#define PLAYER_AIR_CYCLE 9

#define PLAYER_AIR_CYCLE_COUNT 3
extern const unsigned int *air_anime_cycle[];

void init_player();

void unload_player();

void update_player();

#endif