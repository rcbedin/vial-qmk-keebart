#pragma once
#include <stdint.h>
#include <stdbool.h>

extern uint32_t local_presses_left;
extern uint32_t local_presses_right;
extern uint8_t g_oled_max_char;
extern uint8_t g_oled_max_line;

extern uint32_t g_press_left;
extern uint32_t g_press_right;

extern uint8_t g_leftkeys_pressed;
extern uint8_t g_rightkeys_pressed;


/*--------------------------------------
                    ENCODER
----------------------------------------*/
typedef enum  {
    _EC_L_VOLUME = 0,
    _EC_L_MEDIA = 1,
    _EC_L_ZOOM = 2,
    _EC_L_OLED_SPECIAL = 3
} enc_left_layer;

typedef enum  {
    _ER_DEFAULT = 0,
} enc_right_layer;

extern uint8_t l_enc_layer;
extern uint32_t l_enc_sync_time;
extern bool l_enc_pressed;
extern uint32_t l_enc_timer;

extern bool r_enc_pressed;
extern uint32_t r_enc_timer;
/*--------------------------------------*/
