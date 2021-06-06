#include "special_zone.h"

#include <stdio.h>

#include <tonc.h>
#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

#include "../debug.h"

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

static const int _eye_map_row_len = 4;
static const int _eye_map_col_len = 8;
static const unsigned int *_eye_map[] = {
	(unsigned int[]){0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0000},
	(unsigned int[]){0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F},
	(unsigned int[]){0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017},
	(unsigned int[]){0x0000, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E}};

static const FIXED _sz_max_scroll_speed = 0.75f * FIX_SCALEF;
static const int _bg_grid_tile = 1;
static const int _bg_eye00_tile = 7;
static const int _bg_eye01_tile = 40;
static const int _bg_text_tile = 100;
static const int _obj_whale_tile = 0;
static const int _obj_coin_tile = 50;

static sz_data_t _tmp;
static sz_data_t *_data = &_tmp;

static void blink_eye()
{
	if (_data->eyes_looking)
	{
		memcpy16(
			&tile8_mem[SZ_SHARED_CB][_bg_eye00_tile],
			szEye00OpenTiles, szEye00OpenTilesLen / 2);
		memcpy16(
			&tile8_mem[SZ_SHARED_CB][_bg_eye01_tile],
			szEye01OpenTiles, szEye01OpenTilesLen / 2);
	}
	else
	{
		memcpy16(
			&tile8_mem[SZ_SHARED_CB][_bg_eye00_tile],
			szEye00ClosedTiles, szEye00ClosedTilesLen / 2);
		memcpy16(
			&tile8_mem[SZ_SHARED_CB][_bg_eye01_tile],
			szEye01ClosedTiles, szEye01ClosedTilesLen / 2);
	}

	_data->eyes_looking = !_data->eyes_looking;
}

static void make_eye(int x_ofs, int y_ofs)
{
	// Check to see if it's empty space
	for (int y = 0; y < _eye_map_row_len; y++)
	{
		for (int x = 0; x < _eye_map_col_len; x++)
		{
			if (se_mem[SZ_EYE_SBB][32 * (y + y_ofs) + (x + x_ofs)])
				return;
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
		base = 80;
		break;
	case SZ_TS_EYES_OPEN:
		base = 80;
	case SZ_TS_UNFDAING:
	case SZ_TS_FADING:
		base = 7;
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
	// accelerate
	FIXED next_vel = key_held(KEY_A) ? SZ_PLAYER_VELOCITY : -(0.00625f * FIX_SCALEF);
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

		top->x += top->dx;
		top->y += top->dy;

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
				top->enabled = false;
				obj_hide(top->attr);
				break;
			default:
				write_to_log(LOG_LEVEL_DEBUG, "Bad things");
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

static void show(void)
{
	_data->bg0_x = 0 / FIX_SCALE;
	_data->bg0_y = 0 / FIX_SCALE;
	_data->grid_count = 300;
	_data->text_fade_count = 0;
	_data->text_eva = 15;
	_data->text_state = SZ_TS_SOLID;
	_data->grid_toggle = false;
	_data->eyes_looking = false;

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

	memcpy16(
		&tile8_mem[SZ_SHARED_CB][_bg_text_tile],
		szTextTiles, szTextTilesLen / 2);

	// Map
	memset16(&se_mem[SZ_GRID_SBB], _bg_grid_tile, SB_SIZE);
	memset16(&se_mem[SZ_EYE_SBB], 0, SB_SIZE);

	// Obj tiles
	GRIT_CPY(&tile_mem_obj[SZ_SHARED_CB], szWhaleFloat00Tiles);
	GRIT_CPY(&tile_mem_obj[SZ_SHARED_CB][_obj_coin_tile], szGoodCoin00Tiles);

	OAM_CLEAR();
	_data->obj_aff_buffer = (OBJ_AFFINE *)_obj_buffer;

	_data->player.attr = &_obj_buffer[0];
	_data->player.aff = &_data->obj_aff_buffer[0];
	_data->player.x = int2fx(240 / 2 - 16);
	_data->player.y = int2fx(160 / 2 - 16);
	_data->player.angle = 0;
	_data->player.max_velocity = float2fx(0.1f);
	_data->player.velocity = 0;
	_data->player.turning_speed = float2fx(800);
	obj_set_attr(
		_data->player.attr,
		ATTR0_SQUARE | ATTR0_8BPP | ATTR0_AFF | ATTR0_AFF_DBL_BIT,
		ATTR1_SIZE_32 | ATTR1_AFF_ID(_obj_whale_tile),
		ATTR2_PRIO(SZ_OBJECT_LAYER));
	obj_aff_identity(_data->player.aff);

	for (int i = 0; i < SZ_OBS_COUNT; i++)
	{
		_data->obs[i].attr = &_obj_buffer[i + 1];
		_data->obs[i].enabled = true;
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

	// Spawn eyes
	for (int i = 0; i < gba_rand_range(4, 6); i++)
	{
		int x_ofs = gba_rand_range(0, 30 - _eye_map_col_len);
		int y_ofs = gba_rand_range(0, 30 - _eye_map_row_len);
		make_eye(x_ofs, y_ofs);
	}

	for (int i = 0; i < szTextMapLen / 2; i++)
	{
		unsigned int tile = szTextMap[i] ? szTextMap[i] + _bg_text_tile : 0;
		se_mem[SZ_TEXT_SBB][i] = tile;
	}

	// BG regs
	REG_BG0CNT = BG_PRIO(SZ_GRID_LAYER) | BG_8BPP | BG_SBB(SZ_GRID_SBB) | BG_CBB(SZ_SHARED_CB) | BG_REG_32x32;
	REG_BG1CNT = BG_PRIO(SZ_EYE_LAYER) | BG_8BPP | BG_SBB(SZ_EYE_SBB) | BG_CBB(SZ_SHARED_CB) | BG_REG_32x32;
	REG_BG2CNT = BG_PRIO(SZ_TEXT_LAYER) | BG_8BPP | BG_SBB(SZ_TEXT_SBB) | BG_CBB(SZ_SHARED_CB) | BG_REG_32x32;

	// Transparent
	REG_BLDCNT = BLD_BUILD(
		BLD_BG2,					 // Top layers
		BLD_BG0 | BLD_BG1 | BLD_OBJ, // Bottom layers
		1							 // Mode (std)
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

	obj_copy(obj_mem, _obj_buffer, 21);
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
