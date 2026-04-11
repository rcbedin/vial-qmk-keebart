#pragma once
#include <stdint.h>
#include <stdbool.h>

void menu_init(void);
void menu_render(void);

void menu_enter(void);
void menu_exit(void);

void menu_encoder_rotate(bool clockwise);
void menu_encoder_press(void);

bool menu_is_active(void);

bool menu_check_keypress(uint16_t keycode, bool pressed);
void menu_check_usertime(void);

void draw_item(const char* text, bool selected);