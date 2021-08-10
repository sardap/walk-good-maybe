#ifndef ENEMY_H
#define ENEMY_H

#include "ent.h"

#define ENEMY_BISCUT_IDLE_CYCLE 5
extern const unsigned int *enemy_biscut_idle_cycle[];
#define ENEMY_BISCUT_DEATH_CYCLE 4
extern const unsigned int *enemy_biscut_death_cycle[];
#define ENEMY_BISCUT_UFO_IDLE_CYCLE 3
extern const unsigned int *enemy_biscut_ufo_idle_cycle[];
#define ENEMY_BISCUT_UFO_DEATH_CYCLE 11
extern const unsigned int *enemy_biscut_ufo_death_cycle[];

#define ENEMY_BISCUT_SCORE 10
#define ENEMY_UFO_BISCUT_SCORE 15

void load_enemy_bullets_tiles();
void free_enemy_bullets_tiles();

void create_enemy_biscut(ent_t *ent, FIXED x, FIXED y);
void update_enemy_biscut(ent_t *ent);
void free_enemy_biscut(ent_t *ent);

void create_enemy_biscut_death(visual_ent_t *ent, FIXED x, FIXED y);
void update_enemy_biscut_death(visual_ent_t *v_ent);
void free_enemy_biscut_death(visual_ent_t *v_ent);

void create_enemy_ufo_bisuct(ent_t *ent, FIXED x, FIXED y);
void update_enemy_ufo_bisuct(ent_t *ent);
void free_enemy_ufo_bisuct(ent_t *ent);

void create_enemy_ufo_biscut_death(visual_ent_t *ent, FIXED x, FIXED y);
void update_enemy_ufo_biscut_death(visual_ent_t *ent);
void free_enemy_ufo_biscut_death(visual_ent_t *v_ent);

void create_enemy_bullet(ent_t *ent, FIXED x, FIXED y, FIXED vx, FIXED vy);
void update_enemy_bullet(ent_t *ent);

#endif