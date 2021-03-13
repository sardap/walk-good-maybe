#ifndef OBSTACLES_H
#define OBSTACLES_H

#include "ent.h"

void load_speed_up();

void create_speed_up(ent_t *ent, int att_idx, FIXED x, FIXED y);
void create_speed_line(ent_t *ent, int att_idx, FIXED x, FIXED y);

void update_speed_up(ent_t *ent);
void update_speed_line(ent_t *ent);

#endif