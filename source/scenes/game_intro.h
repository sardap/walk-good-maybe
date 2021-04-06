#ifndef GAME_INTRO_SCREEN_H
#define GAME_INTRO_SCREEN_H

#include "../common.h"

#define GI_SHARED_CB 0
#define GI_COOL_BACKGROUND_SSB 8
#define GI_TEXT_SSB 24
#define GI_SKY_SSB 26
#define GI_M7_D 128
#define GI_STARTING_COUNTDOWN 1.65 * 60
//1.65 seconds 60 frames per second
#define GI_WHALE_START_SCALE 512 * FIX_SCALE

typedef enum gi_states_t
{
	GI_S_READY,
	GI_S_SET,
	GI_S_GO,
} gi_states_t;

typedef struct gi_data_t
{
	VECTOR cam_pos;		  // Camera position
	FIXED g_cosf, g_sinf; // cos(phi) and sin(phi), .8f
	u16 cam_phi;
	gi_states_t state;
	OBJ_AFFINE *obj_aff_buffer;
	FIXED whale_scale;
	FIXED whale_rotate;
	int countdown;
	int anime_cycle;
} gi_data_t;

extern const scene_t city_game_intro;
extern const scene_t beach_game_intro;

#endif