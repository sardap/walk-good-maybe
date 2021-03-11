#include "anime.h"

#include <tonc.h>

void step_anime(
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
