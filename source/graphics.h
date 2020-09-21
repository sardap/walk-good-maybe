#ifndef GRAPHICS_H
#include <tonc.h>

typedef struct graphics_into_t {
	int bg_width;
} graphics_into_t;


graphics_into_t get_graphics_into();

void init_graphics();

void load_player_tile();

bool valid_cloud_address(int tile_offset, int sb, int x, int y);

void create_cloud(int tile_offset, int sb, int x, int y);

void place_n_clouds(int tile_offset, int sb, int n);

#endif