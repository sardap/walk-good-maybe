#ifndef ANIME_H
#define ANIME_H

#include <tonc_types.h>

void step_anime_bad(
	const unsigned int *anime[], int tile_len, int count,
	int *cycle, int tile_idx);

//Returns true when the anime is complete
bool step_cb_anime(
	int cb, int *cycle,
	const unsigned int *anime[], int count,
	int tile_idx, int tile_len);

//Returns true when the anime is complete
inline bool step_anime(
	int *cycle,
	const unsigned int *anime[], int count,
	int tile_idx, int tile_len)
{
	return step_cb_anime(4, cycle, anime, count, tile_idx, tile_len);
}

#endif