#ifndef ENEMY_H
#define ENEMY_H

#include "ent.h"

#define ENEMY_TOAST_IDLE_CYCLE 6
extern const unsigned int *enemy_toast_idle_cycle[];

void load_enemy_toast();

void create_toast_enemy(ent_t *ent, int att_idx, FIXED x, FIXED y);

void update_enemy(ent_t *ent);

#endif