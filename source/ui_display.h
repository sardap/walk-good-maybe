#ifndef UI_DISPLAY_H
#define UI_DISPLAY_H

#include "ent.h"

#define SCORE_DIGITS 4

void load_life_display(int life);
void update_life_display(int life);
void free_life_display();

void load_speed_level_display(FIXED speed);
void update_speed_level_display(FIXED speed);
void free_speed_level_display();

void load_jump_level_display(FIXED jump);
void update_jump_level_display(FIXED jump);
void free_jump_level_display();

void init_score_display();
void update_score_display(int score);
void free_score_display();

#endif