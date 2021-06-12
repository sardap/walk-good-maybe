#include "special_zone.h"

#include <stdio.h>

#include <tonc.h>

#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

#include "../debug.h"
#include "../sound.h"
#include "../anime.h"

#include "../assets/szSharedBackground.h"
#include "../assets/szGrid00.h"
#include "../assets/szGrid01.h"
#include "../assets/szEye00Open.h"
#include "../assets/szEye00Closed.h"
#include "../assets/szEye01Open.h"
#include "../assets/szEye01Closed.h"
#include "../assets/szText.h"
#include "../assets/szWhaleFloat00.h"
#include "../assets/szSharedSprite.h"
#include "../assets/szGoodCoin00.h"
#include "../assets/szBadCoin00.h"
#include "../assets/szNumbers.h"
#include "../assets/szUiOverlay.h"
#include "../assets/szLoseSymbol.h"
#include "../assets/szMouth00Open.h"
#include "../assets/szMouth00Closed.h"
#include "../assets/szWhaleFly00.h"
#include "../assets/szWhaleFly01.h"
#include "../assets/szWhaleFly02.h"
#include "../assets/szWhaleSmoke00.h"
#include "../assets/szWhaleSmoke01.h"
#include "../assets/szWhaleSmoke02.h"
#include "../assets/szWhaleSmoke03.h"

static const int _eye_map_row_len = 4;
static const int _eye_map_col_len = 8;
static const unsigned int *_eye_map[] = {
	(unsigned int[]){0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0000},
	(unsigned int[]){0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F},
	(unsigned int[]){0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017},
	(unsigned int[]){0x0000, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E}};

static const int _mouth_map_row_len = 4;
static const int _mouth_map_col_len = 6;
static const unsigned int *_mouth_map[] = {
	(unsigned int[]){0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006},
	(unsigned int[]){0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D},
	(unsigned int[]){0x000E, 0x000F, 0x0010, 0x0011, 0x0012, 0x0013},
	(unsigned int[]){0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019}};

static const int _whale_fly_anime_length = 3;
static const unsigned int *_whale_fly_anime[] = {
	szWhaleFly00Tiles,
	szWhaleFly01Tiles,
	szWhaleFly02Tiles,
};

static const int _whale_smoke_anime_length = 10;
static const unsigned int *_whale_smoke_anime[] = {
	szWhaleSmoke00Tiles,
	szWhaleSmoke01Tiles,
	szWhaleSmoke01Tiles,
	szWhaleSmoke02Tiles,
	szWhaleSmoke02Tiles,
	szWhaleSmoke02Tiles,
	szWhaleSmoke03Tiles,
	szWhaleSmoke03Tiles,
	szWhaleSmoke03Tiles,
	szWhaleSmoke03Tiles,
};

static const FIXED _sz_max_scroll_speed = 0.75f * FIX_SCALEF;
static const int _bg_grid_tile = 1;
static const int _bg_eye00_tile = 7;
static const int _bg_eye01_tile = 40;
static const int _bg_text_tile = 100;
static const int _bg_ui_overlay_tile = 215;
static const int _bg_mouth_tile = _bg_ui_overlay_tile + szUiOverlayTilesLen / 32 + 1;

static const int _obj_whale_tile = 0;
static const int _obj_coin_tile = _obj_whale_tile + szWhaleFloat00TilesLen / 32 + 1;
static const int _obj_numbers_tile = _obj_coin_tile + szGoodCoin00TilesLen / 32 + 1;
static const int _obj_lose_symbol = _obj_numbers_tile + szNumbersTilesLen / 32 + 1;

static sz_data_t _tmp;
static sz_data_t *_data = &_tmp;

static void blink_eye()
{
	if (_data->eyes_looking)
	{
		GRIT_CPY(&tile8_mem[SZ_SHARED_CB][_bg_eye00_tile], szEye00OpenTiles);
		GRIT_CPY(&tile8_mem[SZ_SHARED_CB][_bg_eye01_tile], szEye01OpenTiles);
	}
	else
	{
		GRIT_CPY(&tile8_mem[SZ_SHARED_CB][_bg_eye00_tile], szEye00ClosedTiles);
		GRIT_CPY(&tile8_mem[SZ_SHARED_CB][_bg_eye01_tile], szEye01ClosedTiles);
	}

	_data->eyes_looking = !_data->eyes_looking;
}

static void open_mouth()
{
	// Mouth already open
	if (_data->mouth_open_countdown > 0)
		return;

	_data->mouth_open_countdown = SZ_MOTUH_OPEN_TIME;

	GRIT_CPY(&tile8_mem[SZ_SHARED_CB][_bg_mouth_tile], szMouth00OpenTiles);

	mmEffectEx(&_croc_sound);
}

static void update_mouth()
{
	// Only update if mouth is open
	if (_data->mouth_open_countdown <= 0)
		return;

	_data->mouth_open_countdown--;

	if (_data->mouth_open_countdown <= 0)
		GRIT_CPY(&tile8_mem[SZ_SHARED_CB][_bg_mouth_tile], szMouth00ClosedTiles);
}

static BOOL make_eye(int x_ofs, int y_ofs)
{
	/*
	 Check to see if it's empty space this really doesn't matter if it's slow since
	 it's not called in game loop
	*/
	for (int y = 0; y < _eye_map_row_len; y++)
	{
		for (int x = 0; x < _eye_map_col_len; x++)
		{
			if (se_mem[SZ_EYE_SBB][32 * (y + y_ofs) + (x + x_ofs)])
				return FALSE;
		}
	}

	int tile_offset = gba_rand() % 2 == 0 ? _bg_eye00_tile : _bg_eye01_tile;
	for (int y = 0; y < _eye_map_row_len; y++)
	{
		for (int x = 0; x < _eye_map_col_len; x++)
		{
			unsigned int tile = _eye_map[y][x] ? _eye_map[y][x] + tile_offset : 0;
			se_mem[SZ_EYE_SBB][32 * (y + y_ofs) + (x + x_ofs)] = tile;
		}
	}

	return TRUE;
}

static BOOL make_mouth(int x_ofs, int y_ofs)
{
	/*
	 Check to see if it's empty space this really doesn't matter if it's slow since
	 it's not called in game loop.
	 Would be faster to rollback but this is easier to understand.
	*/
	for (int y = 0; y < _mouth_map_row_len; y++)
	{
		for (int x = 0; x < _mouth_map_col_len; x++)
		{
			if (se_mem[SZ_EYE_SBB][32 * (y + y_ofs) + (x + x_ofs)])
				return FALSE;
		}
	}

	for (int y = 0; y < _mouth_map_row_len; y++)
	{
		for (int x = 0; x < _mouth_map_col_len; x++)
		{
			unsigned int tile = _mouth_map[y][x] ? _mouth_map[y][x] + _bg_mouth_tile : 0;
			se_mem[SZ_EYE_SBB][32 * (y + y_ofs) + (x + x_ofs)] = tile;
		}
	}

	return TRUE;
}

static void update_text_fade(FIXED max)
{
	int base = 0;
	switch (_data->text_state)
	{
	case SZ_TS_FADED:
		base = 240;
		break;
	case SZ_TS_SOLID:
		base = 140;
		break;
	case SZ_TS_EYES_OPEN:
		base = 30;
		break;
	case SZ_TS_UNFDAING:
	case SZ_TS_FADING:
		base = 7;
		break;
	}

	_data->text_fade_count++;

	if (_data->text_fade_count < base + fx2int(fxmul(base * FIX_SCALE, 1 * FIX_SCALE - (fxdiv(max, _sz_max_scroll_speed)))))
		return;

	_data->text_fade_count = 0;

	switch (_data->text_state)
	{
	case SZ_TS_FADED:
		_data->text_state = SZ_TS_UNFDAING;
		REG_DISPCNT |= DCNT_BG2;
		// Switch coins to be bad
		break;
	case SZ_TS_UNFDAING:
		_data->text_eva++;
		if (_data->text_eva >= 15)
		{
			GRIT_CPY(&tile_mem_obj[SZ_SHARED_CB][_obj_coin_tile], szGoodCoin00Tiles);
			_data->text_state = SZ_TS_SOLID;
			blink_eye();
		}
		break;
	case SZ_TS_SOLID:
		_data->text_state = SZ_TS_EYES_OPEN;
		blink_eye();
		_data->text_eva = 13;
		break;
	case SZ_TS_EYES_OPEN:
		_data->text_state = SZ_TS_FADING;
		GRIT_CPY(&tile_mem_obj[SZ_SHARED_CB][_obj_coin_tile], szBadCoin00Tiles);
		break;
	case SZ_TS_FADING:
		_data->text_eva--;
		if (_data->text_eva < 0)
		{
			_data->text_eva = 0;
			REG_DISPCNT ^= DCNT_BG2;
			_data->text_state = SZ_TS_FADED;
		}
		break;
	}

	/* Update blend weights
		Left EVA: Top weight max of 15 (4 bits)
		Right EVB: Bottom wieght max of 15 (4 bits)
		almost complete fade
	*/
	REG_BLDALPHA = BLDA_BUILD(_data->text_eva, 15 - _data->text_eva);
}

static void update_player()
{
	// Rotation
	if (key_tri_shoulder())
	{
		int change = fx2int(fxmul(int2fx(key_tri_shoulder()), _data->player.turning_speed));
		_data->player.angle = WRAP(
			_data->player.angle - change,
			0, 0xFFFF);
		obj_aff_rotate(_data->player.aff, _data->player.angle);
	}

	FIXED next_vel;
	// accelerate
	if (key_held(KEY_A))
	{
		next_vel = SZ_PLAYER_VELOCITY;
		if (frame_count() % 3 == 0)
		{
			_data->player.anime_cycle = WRAP(_data->player.anime_cycle, 0, _whale_fly_anime_length);

			step_anime(
				&_data->player.anime_cycle,
				_whale_fly_anime, _whale_fly_anime_length,
				_obj_whale_tile, szWhaleFloat00TilesLen);
		}
	}
	else
	{
		// Play smoke anime
		if (key_released(KEY_A))
		{
			_data->player.anime_cycle = 0;
			GRIT_CPY(&tile_mem_obj[SZ_SHARED_CB][_obj_whale_tile], szWhaleSmoke00Tiles);
		}

		if (_data->player.anime_cycle < _whale_smoke_anime_length - 1)
		{
			if (frame_count() % 3 == 0)
			{
				step_anime(
					&_data->player.anime_cycle,
					_whale_smoke_anime, _whale_smoke_anime_length,
					_obj_whale_tile, szWhaleSmoke00TilesLen);
			}
		}
		else
		{
			GRIT_CPY(&tile_mem_obj[SZ_SHARED_CB][_obj_whale_tile], szWhaleFloat00Tiles);
		}

		next_vel = SZ_PLAYER_FLOATING_VELOCITY;
	}

	_data->player.velocity = CLAMP(_data->player.velocity + next_vel, 0.0125f * FIX_SCALEF, _data->player.max_velocity);

	FIXED nx = _data->player.x - fxmul(lu_sin(_data->player.angle), _data->player.velocity);
	FIXED ny = _data->player.y - fxmul(lu_cos(_data->player.angle), _data->player.velocity);

	// Check bounds
	if (
		nx + SZ_PLAYER_WIDTH_FX < SCREEN_WIDTH * FIX_SCALE &&
		nx > 0 - fxdiv(SZ_PLAYER_WIDTH_FX, 1.3f * FIX_SCALEF) &&
		ny > 0 - fxdiv(SZ_PLAYER_HEIGHT_FX, 1.3f * FIX_SCALEF) &&
		ny + SZ_PLAYER_HEIGHT_FX < SCREEN_HEIGHT * FIX_SCALE)
	{
		_data->player.x = nx;
		_data->player.y = ny;
	}

	obj_set_pos(_data->player.attr, fx2int(_data->player.x), fx2int(_data->player.y));
}

static void update_score_display(int score)
{
	//Count number of digits
	int w_score = score;
	int digit_count = 0;
	while (w_score != 0)
	{
		digit_count++;
		w_score /= 10;
	}

	w_score = score;
	for (int i = SZ_SCORE_DIGIT_COUNT - 1; i >= 0; i--)
	{
		int offset;
		if (((SZ_SCORE_DIGIT_COUNT - 1) - i) < digit_count)
			offset = (w_score % 10);
		else
			offset = 0;

		w_score /= 10;

		OBJ_ATTR *attr = &_obj_buffer[_data->ui.score_number_offset + i];
		obj_set_attr(
			attr,
			attr->attr0,
			attr->attr1,
			ATTR2_PRIO(SZ_TEXT_LAYER) | ATTR2_ID(_obj_numbers_tile + (offset * 2)));
	}
}

static void create_lose_symbol()
{
	obj_set_attr(
		&_obj_buffer[++_data->obj_count],
		ATTR0_SQUARE | ATTR0_8BPP,
		ATTR1_SIZE_16,
		ATTR2_PRIO(SZ_UI_LAYER) | ATTR2_ID(_obj_lose_symbol));

	int x = 0;
	int y = 0;
	switch (gba_rand_range(0, 3))
	{
	// Left
	case 0:
		x = 0;
		y = gba_rand_range(0, 160 - 16);
		break;
	// Top
	case 1:
		x = gba_rand_range(0, 240 - 16);
		y = 0;
		break;
	// Bottom
	case 2:
		x = gba_rand_range(0, 240 - 16);
		y = 160 - 16;
		break;
	// Right
	case 3:
		x = 240 - 16;
		y = gba_rand_range(0, 160 - 16);
		break;
	}

	obj_set_pos(&_obj_buffer[_data->obj_count], x, y);
}

static void update_obs()
{
	FIXED pl = _data->player.x + fxdiv(SZ_PLAYER_WIDTH_FX, 2 * FIX_SCALE);
	FIXED pr = _data->player.x + fxdiv(SZ_PLAYER_WIDTH_FX, 2 * FIX_SCALE) + SZ_PLAYER_WIDTH_FX;
	FIXED pt = _data->player.y + fxdiv(SZ_PLAYER_HEIGHT_FX, 2 * FIX_SCALE);
	FIXED pb = _data->player.y + fxdiv(SZ_PLAYER_HEIGHT_FX, 2 * FIX_SCALE) + SZ_PLAYER_HEIGHT_FX;

	for (int i = 0; i < SZ_OBS_COUNT; i++)
	{
		sz_obs_t *top = &_data->obs[i];

		if (!top->enabled)
			continue;

		switch (_data->text_state)
		{
		case SZ_TS_SOLID:
		case SZ_TS_EYES_OPEN:
			top->x += top->dx;
			top->y += top->dy;
			break;
		default:
			top->x += fxmul(top->dx, SZ_OBS_SPEED_MULTIPLIER);
			top->y += fxmul(top->dy, SZ_OBS_SPEED_MULTIPLIER);
			break;
		}

		obj_set_pos(top->attr, fx2int(top->x), fx2int(top->y));

		FIXED tl = top->x;
		FIXED tr = top->x + int2fx(8);
		FIXED tt = top->y;
		FIXED tb = top->y + int2fx(8);

		if (RECT_INTERSECTION(pl, pr, pt, pb, tl, tr, tt, tb))
		{
			switch (_data->text_state)
			{
			case SZ_TS_SOLID:
			case SZ_TS_EYES_OPEN:
				top->enabled = FALSE;
				obj_hide(top->attr);
				_data->player.good_collected++;
				update_score_display(_data->player.good_collected);
				break;
			default:
				top->enabled = FALSE;
				obj_hide(top->attr);
				create_lose_symbol();
				open_mouth();
				break;
			}
		}

		if (
			!(
				top->x + SZ_OBS_WIDTH_FX < int2fx(SCREEN_WIDTH) &&
				top->x > 0 &&
				top->y > 0 &&
				top->y + SZ_OBS_HEIGHT_FX < int2fx(SCREEN_HEIGHT)))
		{
			top->dx = -top->dx;
			top->dy = -top->dy;
		}
	}
}

static void update_ui_border()
{
	_data->colour_dist += float2fx(0.05f);

	if (_data->colour_dist >= int2fx(1))
	{
		_data->colour_dist = 0;
		_data->border_colour_current = _data->border_colour_next;
		_data->border_colour_next = (u16)gba_rand();
	}

	FIXED current_red = int2fx(GET_RED(_data->border_colour_current));
	FIXED current_green = int2fx(GET_GREEN(_data->border_colour_current));
	FIXED current_blue = int2fx(GET_BLUE(_data->border_colour_current));

	FIXED next_red = int2fx(GET_RED(_data->border_colour_current));
	FIXED next_geen = int2fx(GET_GREEN(_data->border_colour_next));
	FIXED next_blue = int2fx(GET_BLUE(_data->border_colour_next));

	int result_red = fx2int(current_red + fxmul(_data->colour_dist, next_red - current_red));
	int result_green = fx2int(current_green + fxmul(_data->colour_dist, next_geen - current_green));
	int result_blue = fx2int(current_blue + fxmul(_data->colour_dist, next_blue - current_blue));

	u16 next_colour = CREATE_COLOUR(result_red, result_green, result_blue);

	pal_bg_mem[SZ_BORDER_PAL_IDX] = next_colour;
}

static void show(void)
{
	_data->bg0_x = 0 / FIX_SCALE;
	_data->bg0_y = 0 / FIX_SCALE;
	_data->grid_count = 300;
	_data->text_fade_count = 0;
	_data->text_eva = 15;
	_data->text_state = SZ_TS_SOLID;
	_data->grid_toggle = FALSE;
	_data->eyes_looking = FALSE;
	_data->mouth_open_countdown = 0;

	// Set direction
	_data->bg0_dir_x = float2fx(RAND_FLOAT(0.75f)) + 0.25f * FIX_SCALEF;
	if (gba_rand() % 2 == 0)
		_data->bg0_dir_x = -_data->bg0_dir_x;

	_data->bg0_dir_y = float2fx(RAND_FLOAT(0.75f)) + 0.25f * FIX_SCALEF;
	if (gba_rand() % 2 == 0)
		_data->bg0_dir_x = -_data->bg0_dir_x;

	// Set RegX scroll to 0
	REG_BG0HOFS = fx2int(_data->bg0_x);
	REG_BG0VOFS = fx2int(_data->bg0_y);

	REG_BG1HOFS = fx2int(_data->bg0_x);
	REG_BG1VOFS = fx2int(_data->bg0_y);

	REG_BG3HOFS = fx2int(_data->bg0_x);
	REG_BG3VOFS = fx2int(_data->bg0_y);

	// Load palettes
	GRIT_CPY(pal_bg_mem, szSharedBackgroundPal);
	GRIT_CPY(pal_obj_mem, szSharedSpritePal);

	pal_bg_mem[1] = (u16)gba_rand();
	pal_bg_mem[2] = (u16)gba_rand();

	_data->colour_dist = 0;
	_data->border_colour_current = (u16)gba_rand();
	_data->border_colour_next = (u16)gba_rand();
	update_ui_border();

	// Load background tiles into SZ_SHARED_CB
#ifdef DEBUG
	memset16(
		&tile_mem[SZ_SHARED_CB],
		0, 800 * 64);
#endif
	memcpy16(
		&tile8_mem[SZ_SHARED_CB],
		szGrid01Tiles, szGrid00TilesLen / 2);

	blink_eye();

	GRIT_CPY(&tile8_mem[SZ_SHARED_CB][_bg_text_tile], szTextTiles);
	GRIT_CPY(&tile8_mem[SZ_SHARED_CB][_bg_ui_overlay_tile], szUiOverlayTiles);
	GRIT_CPY(&tile8_mem[SZ_SHARED_CB][_bg_mouth_tile], szMouth00ClosedTiles);

	// Map

	memset16(&se_mem[SZ_GRID_SBB], _bg_grid_tile, SB_SIZE);
	memset16(&se_mem[SZ_EYE_SBB], 0, SB_SIZE);
	memset16(&se_mem[SZ_TEXT_SBB], 0, SB_SIZE);

	// Spawn eyes
	memset16(&se_mem[SZ_EYE_SBB], 0, SB_SIZE);

	int eyes = gba_rand_range(4, 8);

	if (eyes % 2 != 0)
		eyes++;

	for (int i = 0; i < gba_rand_range(3, 7);)
	{
		int x_ofs = gba_rand_range(0, 30 - _eye_map_col_len);
		int y_ofs = gba_rand_range(0, 30 - _eye_map_row_len);
		if (make_eye(x_ofs, y_ofs))
			i++;
	}

	// Spawn mouth
	for (int i = 0; i < eyes / 2; i++)
	{
		int x_ofs = gba_rand_range(0, 30 - _mouth_map_col_len);
		int y_ofs = gba_rand_range(0, 30 - _mouth_map_row_len);
		if (make_mouth(x_ofs, y_ofs))
			i++;
	}

	MAP_COPY(se_mem[SZ_UI_SBB], szUiOverlayMap, _bg_ui_overlay_tile);
	MAP_COPY(se_mem[SZ_TEXT_SBB], szTextMap, _bg_text_tile);

	// Obj tiles
	GRIT_CPY(&tile_mem_obj[SZ_SHARED_CB][_obj_whale_tile], szWhaleFloat00Tiles);
	GRIT_CPY(&tile_mem_obj[SZ_SHARED_CB][_obj_coin_tile], szGoodCoin00Tiles);
	GRIT_CPY(&tile_mem_obj[SZ_SHARED_CB][_obj_numbers_tile], szNumbersTiles);
	GRIT_CPY(&tile_mem_obj[SZ_SHARED_CB][_obj_lose_symbol], szLoseSymbolTiles);

	// Objs
	OAM_CLEAR();
	_data->obj_aff_buffer = (OBJ_AFFINE *)_obj_buffer;

	int top_obj = 0;

	// Player
	_data->player.attr = &_obj_buffer[top_obj];
	_data->player.aff = &_data->obj_aff_buffer[top_obj++];
	_data->player.x = int2fx(240 / 2 - 16);
	_data->player.y = int2fx(160 / 2 - 16);
	_data->player.angle = 0;
	_data->player.max_velocity = float2fx(0.1f);
	_data->player.velocity = 0;
	_data->player.turning_speed = float2fx(800);
	_data->player.good_collected = 0;
	_data->player.anime_cycle = _whale_smoke_anime_length;
	obj_set_attr(
		_data->player.attr,
		ATTR0_SQUARE | ATTR0_8BPP | ATTR0_AFF | ATTR0_AFF_DBL_BIT,
		ATTR1_SIZE_32 | ATTR1_AFF_ID(_obj_whale_tile),
		ATTR2_PRIO(SZ_OBJECT_LAYER));
	obj_aff_identity(_data->player.aff);

	// Obs
	for (int i = 0; i < SZ_OBS_COUNT; i++)
	{
		_data->obs[i].attr = &_obj_buffer[top_obj++];
		_data->obs[i].enabled = TRUE;
		_data->obs[i].x = int2fx(gba_rand_range(0, SCREEN_WIDTH));
		_data->obs[i].y = int2fx(gba_rand_range(0, SCREEN_HEIGHT));
		_data->obs[i].dx = 4 + gba_rand_range(0, 20);
		if (gba_rand() % 2 == 0)
			_data->obs[i].dx = -_data->obs[i].dx;
		_data->obs[i].dy = 4 + gba_rand_range(0, 20);
		if (gba_rand() % 2 == 0)
			_data->obs[i].dy = -_data->obs[i].dy;

		obj_set_attr(
			_data->obs[i].attr,
			ATTR0_SQUARE | ATTR0_8BPP,
			ATTR1_SIZE_8,
			ATTR2_PRIO(SZ_OBJECT_LAYER) | ATTR2_ID(_obj_coin_tile));
	}

	// UI
	_data->ui.score_number_offset = ++top_obj;
	for (int i = 0; i < SZ_SCORE_DIGIT_COUNT; i++)
	{
		obj_set_attr(
			&_obj_buffer[_data->ui.score_number_offset + i],
			ATTR0_SQUARE | ATTR0_8BPP,
			ATTR1_SIZE_8,
			ATTR2_PRIO(SZ_UI_LAYER) | ATTR2_ID(_obj_numbers_tile));
		obj_set_pos(&_obj_buffer[_data->ui.score_number_offset + i], 65 + (i * 10), 10);
	}
	top_obj += SZ_SCORE_DIGIT_COUNT;
	update_score_display(0);

	_data->obj_count = top_obj;

	// BG regs
	REG_BG0CNT = BG_PRIO(SZ_GRID_LAYER) | BG_8BPP | BG_SBB(SZ_GRID_SBB) | BG_CBB(SZ_SHARED_CB) | BG_REG_32x32;
	REG_BG1CNT = BG_PRIO(SZ_EYE_LAYER) | BG_8BPP | BG_SBB(SZ_EYE_SBB) | BG_CBB(SZ_SHARED_CB) | BG_REG_32x32;
	REG_BG2CNT = BG_PRIO(SZ_TEXT_LAYER) | BG_8BPP | BG_SBB(SZ_TEXT_SBB) | BG_CBB(SZ_SHARED_CB) | BG_REG_32x32;
	REG_BG3CNT = BG_PRIO(SZ_UI_LAYER) | BG_8BPP | BG_SBB(SZ_UI_SBB) | BG_CBB(SZ_SHARED_CB) | BG_REG_32x32;

	// Transparent
	REG_BLDCNT = BLD_BUILD(
		BLD_BG2,							   // Top layers
		BLD_BG0 | BLD_BG1 | BLD_BG3 | BLD_OBJ, // Bottom layers
		1									   // Mode (std)
	);

	/* Update blend weights
		Left EVA: Top weight max of 15 (4 bits)
		Right EVB: Bottom wieght max of 15 (4 bits)
		almost complete fade
	*/
	REG_BLDALPHA = BLDA_BUILD(_data->text_eva, 15 - _data->text_eva);
	REG_BLDY = BLDY_BUILD(0);

	update_text_fade(1);

	// enable hblank register and set the mode7 type
	irq_init(NULL);
	irq_add(II_VBLANK, mmVBlank);

	REG_DISPCNT = DCNT_MODE0 | DCNT_OBJ | DCNT_OBJ_1D | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_BG3;
}

static void update(void)
{
	if (frame_count() % 15 == 0)
		update_ui_border();

	_data->grid_count++;

	// Grid
	FIXED max = MAX(_data->bg0_dir_x, _data->bg0_dir_y) - 0.25f * FIX_SCALEF;

	if (_data->grid_count > 10 + fx2int(fxmul(30 * FIX_SCALE, 1 * FIX_SCALE - (fxdiv(max, _sz_max_scroll_speed)))))
	{
		if (_data->grid_toggle)
			memcpy16(
				&tile_mem[SZ_SHARED_CB],
				szGrid00Tiles, szGrid00TilesLen);
		else
			memcpy16(
				&tile_mem[SZ_SHARED_CB],
				szGrid01Tiles, szGrid01TilesLen);

		_data->grid_toggle = !_data->grid_toggle;

		_data->grid_count = 0;
	}

	_data->bg0_x += _data->bg0_dir_x;
	_data->bg0_y += _data->bg0_dir_y;

	REG_BG0HOFS = fx2int(_data->bg0_x);
	REG_BG0VOFS = fx2int(_data->bg0_y);

	// Eyes
	REG_BG1HOFS = -fx2int(fxmul(_data->bg0_x, 0.5f * FIX_SCALEF));
	REG_BG1VOFS = -fx2int(fxmul(_data->bg0_y, 0.5f * FIX_SCALEF));

	// Text
	update_text_fade(max);

	// Objs

	update_player();
	update_obs();
	update_mouth();

	obj_copy(obj_mem, _obj_buffer, _data->obj_count + 1);
	obj_aff_copy(obj_aff_mem, _data->obj_aff_buffer, 1);
}

static void hide(void)
{
	REG_DISPCNT = 0;

	REG_BLDCNT = BLD_BUILD(
		BLD_OFF, // Top layers
		BLD_OFF, // Bottom layers
		BLD_OFF	 // Mode (off)
	);
}

const scene_t special_zone_scene = {
	.show = show,
	.update = update,
	.hide = hide};
