#ifndef GUN_H
#define GUN_H

#include "ent.h"

void load_gun_0_tiles();
void free_gun_0_tiles();

int gun_0_tiles();

void create_bullet(ent_t *bul, bullet_type_t type, FIXED x, FIXED y, FIXED vx, FIXED vy, int flip);
void update_bullet(ent_t *bul);

#endif