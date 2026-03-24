#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    _SCR_LEFT  = 0,
    _SCR_RIGHT = 1
} e_oled_screen;

void widgets_init(void);

void widgets_render(e_oled_screen screen);

void widgets_add(void);
void widgets_remove(void);
