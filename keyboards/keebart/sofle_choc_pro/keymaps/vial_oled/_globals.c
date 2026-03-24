#include "_globals.h"
#include "quantum.h"

// static const uint8_t OLED_WIDTH = OLED_DISPLAY_HEIGHT;
// static const uint16_t SPLASH_DURATION_MS = 2500;

uint32_t local_presses_left = 0;
uint32_t local_presses_right = 0;
uint8_t g_oled_max_char = 0;
uint8_t g_oled_max_line = 0;

uint32_t g_press_left = 0;
uint32_t g_press_right = 0;

uint8_t g_leftkeys_pressed = 0;
uint8_t g_rightkeys_pressed = 0;