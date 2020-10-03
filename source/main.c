#include <tonc.h>

#include "debug.h"
#include "common.h"
#include "graphics.h"
#include "ent.h"

void game_loop() {
	while(1)
	{
		vid_vsync();
		key_poll();

		common_step();

		scene_update();
	}
}

void start_game() {
	scene_set(title_screen);
	game_loop();
}

int main()
{
#ifdef DEBUG
	init_debug();
#endif
	oam_init(_obj_buffer, 128);

	init_graphics();

	start_game();

    while(1);

    return 0;
}
