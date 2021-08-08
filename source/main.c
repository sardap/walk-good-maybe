#include <tonc.h>
#include <maxmod.h>
#include "soundbank_bin.h"

#include "debug.h"
#include "common.h"
#include "graphics.h"
#include "ent.h"
#include "scenes/title_screen.h"
#include "scenes/warning.h"

void game_loop()
{
	irq_init(NULL);
	irq_enable(II_VBLANK);

	irq_add(II_VBLANK, mmVBlank);
	mmInitDefault((mm_addr)soundbank_bin, 8);

	while (1)
	{
		VBlankIntrWait();

		mmFrame();
		key_poll();

		common_step();

		scene_update();
	}
}

void start_game()
{
	scene_set(title_screen);
	game_loop();
}

int main()
{
#ifdef DEBUG
	init_debug();
#endif

#ifdef SHOW_WARNING
	show_warning();
#endif

	init_all_ents();

	init_graphics();

	start_game();

	for (;;)
	{
	}

	return 0;
}
