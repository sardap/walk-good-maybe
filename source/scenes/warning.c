#include "warning.h"

#include <tonc.h>

#include "../common.h"
#include "../assets/wrMain.h"
#include "../assets/wrSharedBackground.h"
#include "../assets/wrButton.h"

void show_warning()
{
	// init
	irq_init(NULL);
	irq_enable(II_VBLANK);

	GRIT_CPY(pal_bg_mem, wrSharedBackgroundPal);
	GRIT_CPY(&tile8_mem[WR_SHARED_CB], wrSharedBackgroundTiles);
	GRIT_CPY(&se_mem[WR_BACKGROUND_SBB], wrMainMap);
	GRIT_CPY(&se_mem[WR_BUTTON_SBB], wrButtonMap);

	REG_BG0HOFS = 0;
	REG_BG0VOFS = 0;

	REG_BG1HOFS = 0;
	REG_BG1VOFS = 0;

	REG_BG0CNT = BG_PRIO(0) | BG_8BPP | BG_SBB(WR_BACKGROUND_SBB) | BG_CBB(WR_SHARED_CB) | BG_REG_32x32;
	REG_BG1CNT = BG_PRIO(0) | BG_8BPP | BG_SBB(WR_BUTTON_SBB) | BG_CBB(WR_SHARED_CB) | BG_REG_32x32;

	REG_DISPCNT = DCNT_MODE1 | DCNT_BG0;

	// loop
	for (int i = 0; i < 3 * 60; i++)
	{
		VBlankIntrWait();
		key_poll();
	}

	REG_DISPCNT |= DCNT_BG1;

	while (!key_hit(KEY_A))
	{
		VBlankIntrWait();
		key_poll();
	}
}