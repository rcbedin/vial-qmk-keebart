#pragma once
#include <stdint.h>
#include <stdbool.h>

extern uint32_t local_presses_left;
extern uint32_t local_presses_right;
extern uint8_t g_oled_max_char;
extern uint8_t g_oled_max_line;

uint32_t g_press_left;
uint32_t g_press_right;

uint8_t g_leftkeys_pressed;
uint8_t g_rightkeys_pressed;