#ifndef OBSTACLES_H
#define OBSTACLES_H

#include "ent.h"

void load_speed_up();
void load_health_up();
void load_jump_up();

void create_speed_up(ent_t *ent, int ent_idx, FIXED x, FIXED y);
void update_speed_up(ent_t *ent);

void create_speed_line(visual_ent_t *ent, int ent_idx, FIXED x, FIXED y);
void update_speed_line(visual_ent_t *ent);

void create_health_up(ent_t *ent, int ent_idx, FIXED x, FIXED y);
void update_health_up(ent_t *ent);

void create_jump_up(ent_t *ent, int ent_idx, FIXED x, FIXED y);
void update_jump_up(ent_t *ent);

#endif