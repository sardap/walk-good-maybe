#include "anime.h"

#include <tonc.h>

void step_anime_bad(
	const unsigned int *anime[], int tile_len, int count,
	int *cycle, int tile_idx)
{
	//God I wish I wrote comments explaning this
	if ((*cycle) % count == 0)
	{
		dma3_cpy(
			&tile_mem[4][tile_idx],
			anime[(*cycle) / count],
			tile_len);
	}

	(*cycle)--;
	if ((*cycle) < 0)
	{
		(*cycle) = count * 3;
	}
}

bool step_cb_anime(
	int cb, int *cycle,
	const unsigned int *anime[], int count,
	int tile_idx, int tile_len)
{
	//Copy current frame into tile mem
	dma3_cpy(
		&tile_mem[cb][tile_idx],
		anime[(*cycle)],
		tile_len);

	//Next frame
	++(*cycle);

	//Check if need to wrap cycle
	if ((*cycle) >= count)
	{
		(*cycle) = 0;
		return true;
	}

	return false;
}