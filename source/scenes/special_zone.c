#include "special_zone.h"

#include <stdio.h>

#include <tonc.h>
#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

#include "../assets/szBanditFace00.h"
#include "../assets/szSharedBackground.h"

static void show(void)
{
	// Set RegX scroll to 0
	REG_BG0HOFS = 0;
	REG_BG0VOFS = 0;

	/* Load palettes */
	GRIT_CPY(pal_bg_mem, szSharedBackgroundPal);
	/* Load background tiles into SZ_SHARED_CB */
	//reg background
	GRIT_CPY(&tile_mem[SZ_SHARED_CB], szSharedBackgroundTiles);
	//Map
	GRIT_CPY(se_mem[SZ_BANDIT_FACE_SBB], szBanditFace00Map);

	REG_BG0CNT = BG_PRIO(0) | BG_8BPP | BG_SBB(SZ_BANDIT_FACE_SBB) | BG_CBB(SZ_SHARED_CB) | BG_REG_32x32;

	// // and vblank int for vsync
	irq_add(II_HBLANK, NULL);
	irq_add(II_VBLANK, mmVBlank);

	REG_DISPCNT = DCNT_BG0;
}

static void update(void)
{
}

static void hide(void)
{
	REG_DISPCNT = 0;
}

const scene_t special_zone_scene = {
	.show = show,
	.update = update,
	.hide = hide};
