#pragma once
#include <stdint.h>
#include <stdbool.h>

uint16_t get_current_dwpm(void);
uint8_t round_percentage(float x);
void oled_blit_16x16_P(const char *icon, uint8_t x, uint8_t page);
void oled_blit_24x24_P(const char *icon, uint8_t x, uint8_t page);
void oled_print_right_aligned(const char *text, const uint8_t width);