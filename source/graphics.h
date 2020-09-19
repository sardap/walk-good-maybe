#ifndef GRAPHICS_H

typedef struct graphics_into_t {
	int bg_width;
} graphics_into_t;

graphics_into_t get_graphics_into();

void init_graphics();

void load_player_tile();

#endif