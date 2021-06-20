#include "sound.h"

#include "soundbank.h"
#include "soundbank_bin.h"

//Player
mm_sound_effect _player_damage = {
	{SFX_WHALE_DAMGE},
	(int)(1.0f * (1 << 10)),
	0,
	120,
	127,
};

mm_sound_effect _player_shoot_sound = {
	{SFX_BY_LASER_4},
	(int)(1.0f * (1 << 10)),
	PLAYER_ACTION_SOUND_HANDLER,
	120,
	127,
};

mm_sound_effect _player_jump_sound = {
	{SFX_BY_JUMP_2},
	(int)(1.0f * (1 << 10)),
	PLAYER_ACTION_SOUND_HANDLER,
	120,
	127,
};

mm_sound_effect _player_land_sound = {
	{SFX_BY_BONK_1},
	(int)(1.0f * (1 << 10)),
	PLAYER_ACTION_SOUND_HANDLER,
	120,
	127,
};

mm_sound_effect _player_walk_sound = {
	{SFX_H6_FEET_27},
	(int)(1.0f * (1 << 10)),
	PLAYER_WALK_SOUND_HANDLER,
	70,
	127,
};

mm_sound_effect _player_flap_sound = {
	{SFX_FLAP_0},
	(int)(1.0f * (1 << 10)),
	PLAYER_WALK_SOUND_HANDLER,
	70,
	127,
};

mm_sound_effect _player_shrink_sound = {
	{SFX_BY_R_COLLECT_7},
	(int)(1.0f * (1 << 10)),
	TOKEN_SOUND_HANDLER,
	120,
	127,
};

mm_sound_effect _player_grow_sound = {
	{SFX_BY_COLLECT_7},
	(int)(1.0f * (1 << 10)),
	TOKEN_SOUND_HANDLER,
	120,
	127,
};

//Enemy
mm_sound_effect _enemy_biscut_damage = {
	{SFX_PD_ENEMY_0_DEATH_0},
	(int)(1.0f * (1 << 10)),
	ENEMY_SOUND_HANDLER,
	120,
	127,
};

mm_sound_effect _enemy_ufo_damge = {
	{SFX_ENEMY_BISCUT_UFO_DEATH_0},
	(int)(1.0f * (1 << 10)),
	ENEMY_SOUND_HANDLER,
	120,
	127,
};

mm_sound_effect _enemy_shoot = {
	{SFX_BY_GUN_4},
	(int)(1.0f * (1 << 10)),
	ENEMY_SOUND_HANDLER,
	50,
	127,
};

//Misc
mm_sound_effect _health_pick_up_sound = {
	{SFX_BY_COLLECT_4},
	(int)(1.0f * (1 << 10)),
	TOKEN_SOUND_HANDLER,
	170,
	127,
};

mm_sound_effect _jump_pick_up_sound = {
	{SFX_BY_COLLECT_5},
	(int)(1.0f * (1 << 10)),
	TOKEN_SOUND_HANDLER,
	120,
	127,
};

mm_sound_effect _speed_pick_up_sound = {
	{SFX_JDW_BLOW_1},
	(int)(1.0f * (1 << 10)),
	TOKEN_SOUND_HANDLER,
	120,
	127,
};

//Voice lines
mm_sound_effect _ready_sound = {
	{SFX_READY_0},
	(int)(1.0f * (1 << 10)),
	GI_INTRO_HANDLER,
	120,
	127,
};

mm_sound_effect _set_sound = {
	{SFX_SET_0},
	(int)(1.0f * (1 << 10)),
	GI_INTRO_HANDLER,
	120,
	127,
};

mm_sound_effect _go_sound = {
	{SFX_GO_0},
	(int)(1.0f * (1 << 10)),
	GI_INTRO_HANDLER,
	120,
	127,
};

mm_sound_effect _always_backwards_sound = {
	{SFX_EG_ALWAYS},
	(int)(1.0f * (1 << 10)),
	MISC_SOUND_HANDLER,
	120,
	127,
};

// Animal Sounds
mm_sound_effect _croc_sound = {
	{SFX_TZ_CROC_GROAL},
	(int)(1.0f * (1 << 10)),
	MISC_SOUND_HANDLER,
	120,
	127,
};

const sound_fx_info_t _sound_fx_set[] = {
	{&_player_damage, "Player Dmg"},
	{&_player_shoot_sound, "Player Shoot"},
	{&_player_jump_sound, "Player Jump"},
	{&_player_land_sound, "Player Land"},
	{&_player_walk_sound, "Player Walk"},
	{&_player_flap_sound, "Player Flap"},
	{&_player_shrink_sound, "Player Shrink"},
	{&_player_grow_sound, "Player Grow"},
	{&_enemy_biscut_damage, "Emy Bisc Dmg"},
	{&_enemy_ufo_damge, "Emy UFO Bisc Dmg"},
	{&_enemy_shoot, "Emy Shoot"},
	{&_health_pick_up_sound, "Health Token"},
	{&_jump_pick_up_sound, "Jump Token"},
	{&_speed_pick_up_sound, "Speed Token"},
	{&_ready_sound, "Voice Ready"},
	{&_set_sound, "Voice Set"},
	{&_go_sound, "Voice Go"},
	{&_croc_sound, "Croc Groal"},
	{&_always_backwards_sound, "Always Backwards"},
};