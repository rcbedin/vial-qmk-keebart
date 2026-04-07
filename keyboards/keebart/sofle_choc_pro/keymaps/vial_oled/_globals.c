#include "_globals.h"
#include "quantum.h"

static uint8_t g_oled_max_char = 0;
static uint8_t g_oled_max_line = 0;

static uint32_t g_total_press_left = 0;
static uint32_t g_total_press_right = 0;


static uint32_t g_remote_total_press_left = 0;
static uint32_t g_remote_total_press_right = 0;

static uint32_t g_current_leftkeys_pressed = 0;
static uint32_t g_current_rightkeys_pressed = 0;

void init_globals(void) {}

void update_oled_limits(void) {
    g_oled_max_char = oled_max_chars();
    g_oled_max_line = oled_max_lines();
}

uint8_t get_oled_limit(char id) {
    switch (id)
    {
    case 'c':
        return g_oled_max_char;
        break;
    case 'l': 
        return g_oled_max_line;
        break;
    default:
        return 0;
        break;
    }
}

presses_m2s_t update_pressed_key_counters(char side, bool pressed) {
    switch (side)
    {
        case 'l':            
            if (pressed) {
                g_total_press_left++;
                g_current_leftkeys_pressed++;
            } else {
                if (g_current_leftkeys_pressed > 0) {
                    g_current_leftkeys_pressed--;
                }
            }
            break;
        case 'r': 
            if (pressed) {
                g_total_press_right++;
                g_current_rightkeys_pressed++;
            } else {
                if (g_current_rightkeys_pressed > 0) {
                    g_current_rightkeys_pressed--;
                }
            }
            break;
        default:            
            break;
    }

    return (presses_m2s_t){g_total_press_left,g_total_press_right};       
}

void update_remote_presses(presses_m2s_t input) {
    g_remote_total_press_left = input.left;
    g_remote_total_press_right = input.right; 
}

presses_m2s_t get_total_presses_count(void){
    if (is_keyboard_master()) {
        return (presses_m2s_t){g_total_press_left, g_total_press_right};
    } else {
        return (presses_m2s_t){g_remote_total_press_left, g_remote_total_press_right};
    }
}

presses_m2s_t get_current_pressed(void) {
    return (presses_m2s_t){g_current_leftkeys_pressed, g_current_rightkeys_pressed};
}

/*--------------------------------------
                    ENCODER
----------------------------------------*/
uint8_t l_enc_layer = _EC_L_VOLUME;
uint32_t l_enc_sync_time = 0;

bool l_enc_pressed = false;
uint32_t l_enc_timer = 0;

bool r_enc_pressed = false;
uint32_t r_enc_timer = 0;
/*--------------------------------------*/
