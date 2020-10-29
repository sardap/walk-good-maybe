#ifndef GUN_H
#define GUN_H

#include "ent.h"

void load_gun_0_tiles();
void unload_gun_0_tiles();

void create_bullet(bullet_type_t type, FIXED x, FIXED y, FIXED vx, FIXED vy);
void update_bullets();

#endif