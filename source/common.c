#include "common.h"

#include <stdlib.h>
#include <tonc.h>

#include "debug.h"
#include "numbers.h"

static int _frame_count;
FIXED _scroll_x;
static int _score;
static int _score_ent_start;

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
	_score_ent_start = allocate_ent(SCORE_DIGITS);

	for (int i = 0; i < SCORE_DIGITS; i++)
	{
		_ents[_score_ent_start + i].ent_type = TYPE_LIFE;

		_ents[_score_ent_start + i].att.attr0 = ATTR0_SQUARE | ATTR0_8BPP;
		_ents[_score_ent_start + i].att.attr1 = ATTR1_SIZE_8x8;
		_ents[_score_ent_start + i].att.attr2 = ATTR2_PALBANK(0) | ATTR2_PRIO(0) | ATTR2_ID(get_number_tile_start());

		_ents[_score_ent_start + i].x = int2fx(8 * i);
		_ents[_score_ent_start + i].y = 0;
	}
}

static void update_score()
{
	int i_score = _score;

	int digit_count = 0;
	while (i_score != 0)
	{
		digit_count++;
		i_score /= 10;
	}

	i_score = _score;
	for (int i = SCORE_DIGITS - 1; i >= 0; i--)
	{
		int offset;
		if (((SCORE_DIGITS - 1) - i) < digit_count)
		{
			offset = (i_score % 10);
		}
		else
		{
			offset = 0;
		}

		i_score /= 10;

		_ents[_score_ent_start + i].att.attr2 =
			ATTR2_PALBANK(0) |
			ATTR2_PRIO(0) |
			ATTR2_ID(get_number_tile_start() + offset * 2);
	}
}

void clear_score()
{
	free_ent(_score_ent_start, SCORE_DIGITS);
}

void add_score(int x)
{
	if (x == 0)
	{
		return;
	}

	_score += x;
	update_score();
}

int get_score()
{
	return _score;
}
