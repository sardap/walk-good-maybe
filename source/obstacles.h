#ifndef OBSTACLES_H
#define OBSTACLES_H

#include "ent.h"

void load_speed_up();
void free_speed_up();

void load_health_up();
void free_health_up();

void load_jump_up();
void free_jump_up();

void load_shrink_token();
void free_shrink_token();

void create_speed_up(ent_t *ent, FIXED x, FIXED y);
void update_speed_up(ent_t *ent);

void create_speed_line(visual_ent_t *ent, FIXED x, FIXED y);
void update_speed_line(visual_ent_t *ent);

void create_health_up(ent_t *ent, FIXED x, FIXED y);
void update_health_up(ent_t *ent);

void create_jump_up(ent_t *ent, FIXED x, FIXED y);
void update_jump_up(ent_t *ent);

void create_shrink_token(ent_t *ent, FIXED x, FIXED y);
void update_shrink_token(ent_t *ent);

void create_speical_zone_portal(ent_t *ent, FIXED x, FIXED y);
void update_speical_zone_portal(ent_t *ent);
void free_speical_zone_portal(ent_t *ent);

#endif