#include "common.h"

#include <stdlib.h>
#include <tonc.h>

#include "debug.h"
#include "numbers.h"
#include "ent.h"
#include "ui_display.h"

static int _frame_count;
FIXED _scroll_x;
static int _score;

static void nothing(void) {}

static scene_t next_scene = {
	.show = nothing,
	.update = nothing,
	.hide = nothing};

static scene_t current_scene = {
	.show = nothing,
	.update = nothing,
	.hide = nothing};

static bool scene_changed = false;

void common_step()
{
	_frame_count++;
}

int frame_count()
{
	return _frame_count;
}

void scene_set(scene_t scene)
{
	scene_changed = true;
	next_scene = scene;
}

void scene_update()
{
	if (scene_changed)
	{
		scene_changed = false;
		current_scene.hide();
		current_scene = next_scene;
		current_scene.show();
	}
	current_scene.update();
}

void init_seed(int seed)
{
	srand((unsigned int)seed);
}

int gba_rand()
{
	return qran();
}

void init_score()
{
	_score = 0;

	init_score_display();
}

void add_score(int x)
{
	if (x == 0)
	{
		return;
	}

	_score += x;
	update_score_display(get_score());
}

int get_score()
{
	return _score;
}

void load_blank()
{
	memset16(pal_bg_mem, 0, PAL_BG_SIZE / 2);
	REG_BG0CNT = BG_PRIO(2) | BG_8BPP | BG_SBB(24) | BG_CBB(0) | BG_REG_32x32;
	REG_DISPCNT = DCNT_BG0;
	VBlankIntrWait();
}
