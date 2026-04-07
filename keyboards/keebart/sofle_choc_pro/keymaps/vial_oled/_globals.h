#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t left;
    uint32_t right;
} presses_m2s_t;

void init_globals(void);
void update_oled_limits(void);
uint8_t get_oled_limit(char id);

presses_m2s_t update_pressed_key_counters(char side, bool pressed);
void update_remote_presses(presses_m2s_t input);

presses_m2s_t get_total_presses_count(void);
presses_m2s_t get_current_pressed(void);

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
