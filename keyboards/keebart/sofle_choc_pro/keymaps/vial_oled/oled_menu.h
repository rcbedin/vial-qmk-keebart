#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    MENU_OFF,
    MENU_MAIN,
    MENU_RGB,
    MENU_RGB_ANIM,
    MENU_RGB_COLOR,
    MENU_LEFT_ANIM,
    MENU_RIGHT_ANIM
} menu_state_t;


typedef struct {
    menu_state_t state;
    uint8_t index;
} menu_sync_m2s_t;

void menu_init(void);
void menu_render(void);

void menu_enter(void);
void menu_exit(void);

void menu_encoder_rotate(bool clockwise);
void menu_encoder_press(void);

bool menu_is_active(void);

bool menu_check_keypress(uint16_t keycode, bool pressed);
void menu_check_usertime(void);

//REMOTE EVENTS TO THE CLIENT
void menu_handle_state_from_remote(uint8_t in_len, const void* in_data);
void menu_handle_mov_from_remote(uint8_t in_len, const void* in_data);

void draw_item(const char* text, bool selected);