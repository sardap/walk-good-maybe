#ifndef SOUND_H
#define SOUND_H

#define GI_INTRO_HANDLER (mm_sfxhand)1
#define PLAYER_ACTION_SOUND_HANDLER (mm_sfxhand)2
#define PLAYER_WALK_SOUND_HANDLER (mm_sfxhand)3
#define ENEMY_SOUND_HANDLER (mm_sfxhand)4
#define TOKEN_SOUND_HANDLER (mm_sfxhand)5
#define CROC_SOUND_HANDLER (mm_sfxhand)6

#include <mm_types.h>

typedef struct sound_fx_info_t
{
	mm_sound_effect *sound;
	const char *name;
} sound_fx_info_t;

//Player
extern mm_sound_effect _player_damage;
extern mm_sound_effect _player_shoot_sound;
extern mm_sound_effect _player_jump_sound;
extern mm_sound_effect _player_land_sound;
extern mm_sound_effect _player_walk_sound;
extern mm_sound_effect _player_flap_sound;
extern mm_sound_effect _player_shrink_sound;
extern mm_sound_effect _player_grow_sound;

//Enemy
extern mm_sound_effect _enemy_biscut_damage;
extern mm_sound_effect _enemy_ufo_damge;
extern mm_sound_effect _enemy_shoot;

//Misc
extern mm_sound_effect _health_pick_up_sound;
extern mm_sound_effect _jump_pick_up_sound;
extern mm_sound_effect _speed_pick_up_sound;

//Voice lines
extern mm_sound_effect _ready_sound;
extern mm_sound_effect _set_sound;
extern mm_sound_effect _go_sound;

// Animal noises
extern mm_sound_effect _croc_sound;

#define SOUND_SET_LENGTH 17
extern const sound_fx_info_t _sound_fx_set[SOUND_SET_LENGTH];

#endif