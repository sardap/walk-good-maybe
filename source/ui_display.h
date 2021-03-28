#ifndef UI_DISPLAY_H
#define UI_DISPLAY_H

#include "ent.h"

void load_life_display();
void update_life_display(int life);
void unload_life_display();

void load_speed_level_display();
void update_speed_level_display(int life);

void init_jump_level_display();
void update_jump_level_display(int life);
void free_jump_level_display();

#endif