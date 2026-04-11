#include "quantum.h"
#include "screen_renderer.h"
#include "eeconfig.h"
#include "bitmaps.h"
#include "_storage.h"
#include "_utils.h"
#include "_globals.h"
#include "animations/bongo_cat.h"
#include "oled_menu.h"

enum layers { _BASE = 0, _LOWER = 1, _RAISE = 2 };

void widget_kbd_lock(void) {
    led_t led_state = host_keyboard_led_state();
    if (led_state.num_lock) {
        oled_blit_16x16_P(NUM_LOCK_BITMAP, 0, 0);
    } else {
        oled_blit_16x16_P(EMPTY_BITMAP, 0, 0);
    }
    if (led_state.caps_lock) {
        oled_blit_16x16_P(CAPS_LOCK_BITMAP, 24, 0);
    } else {
        oled_blit_16x16_P(EMPTY_BITMAP, 24, 0);
    }
    if (led_state.scroll_lock) {
        oled_blit_16x16_P(SCROLL_LOCK_BITMAP, 48, 0);
    } else {
        oled_blit_16x16_P(EMPTY_BITMAP, 48, 0);
    }
}

uint8_t widget_current_layer(uint8_t start_row) {
    oled_set_cursor(0, start_row);

    switch (get_highest_layer(layer_state)) {
        case _BASE:
            oled_write_P(BASE_LAYER_BMP, false);
            break;
        case _LOWER:
            oled_write_P(LOWER_LAYER_BMP, false);
            break;
        case _RAISE:
            oled_write_P(UPPER_LAYER_BMP, false);
            break;
        default:
            // TODO: mmake something with other layers that we could have
            oled_write_P(BASE_LAYER_BMP, false);
            // TODO: consider remove snprintf
            // snprintf(layer_str, sizeof(layer_str), "%d", get_highest_layer(layer_state));
    }

    return start_row + 1;
}

uint8_t widget_uptime(uint8_t start_row) {
    uint32_t time_ms   = timer_read32();
    uint32_t total_min = time_ms / 60000u;
    uint32_t hours     = total_min / 60u;
    uint32_t minutes   = total_min % 60u;
    if (hours > 999u) {
        hours   = 999u;
        minutes = 59u;
    }
    oled_set_cursor(0, start_row++);
    oled_write_P(PSTR("Uptime:"), false);

    // TODO: consider remove snprintf
    char buf[8];
    snprintf(buf, sizeof(buf), "%3luh%02lum", hours, minutes);
    oled_set_cursor(0, start_row++);
    oled_print_right_aligned(buf, get_oled_limit('c'));

    return start_row++;
}

uint8_t widget_avg_speed(uint8_t start_row) {
    oled_set_cursor(0, start_row++);
    oled_write_P(PSTR("Avg Speed"), false);
    oled_set_cursor(0, start_row++);
    oled_write_P(PSTR("(25 s):"), false);

    return start_row++;
}

uint8_t widget_wpm(uint8_t start_row) {
    uint16_t wpm      = get_current_dwpm();
    uint16_t wpm_int  = wpm / 10u;
    uint16_t wpm_frac = wpm % 10u;

    // TODO: consider remove snprintf
    char buf[11];
    snprintf(buf, sizeof(buf), "%3u.%1u WPM", wpm_int, wpm_frac);
    oled_set_cursor(0, start_row++);
    oled_print_right_aligned(buf, get_oled_limit('c'));

    return start_row++;
}

uint8_t widget_split_balance(uint8_t start_row, const uint32_t *presses_qt, const char *side) {   
    // uint32_t total = get_presses('l') + get_presses('r');
    presses_m2s_t presses = get_total_presses_count();
    uint32_t total = presses.left + presses.right;

    if (total == 0) {
        total = 1;  // avoid div by 0
    }

    uint8_t pct_calc = round_percentage((100.0f * (*presses_qt)) / total);

    oled_set_cursor(0, start_row++);
    oled_write_P(side, false);

    char buf[6];
    snprintf(buf, sizeof(buf), "%3u %%", pct_calc);
    oled_set_cursor(0, start_row++);
    oled_print_right_aligned(buf, get_oled_limit('c'));

    return start_row++;
}

void widget_left_encoder_layer(uint8_t start_row){
    oled_set_cursor(0,start_row++);

    switch(l_enc_layer) {
        case _EC_L_VOLUME: 
            oled_write_P(PSTR("Volume"), false);
            oled_set_cursor(0,start_row++);
            oled_print_right_aligned(PSTR("[-] [+]"), get_oled_limit('c'));
            // oled_write_P(BONGO_R1, false);
            break;
        case _EC_L_MEDIA:
            oled_write_P(PSTR("Media"), false);
            oled_set_cursor(0,start_row++);
            oled_print_right_aligned(PSTR("[<] [>]"), get_oled_limit('c'));
            break;

        case _EC_L_ZOOM: 
            oled_write_P(PSTR("Zoom"), false);
            oled_set_cursor(0,start_row++);
            oled_print_right_aligned(PSTR("[-] [+]"), get_oled_limit('c'));
            break;
        default: 
        break;
    }
}

uint8_t widget_bongo_cat(uint8_t start_row) {
    for (int i = 0; i < 3; i++) {
        oled_set_cursor(0, start_row + i);
        oled_write_P(anim_bongocat_get_frame(i), false);
    }

    return start_row ;
}

void widgets_init(void) {}

void widgets_render(e_oled_screen screen) {
   
    presses_m2s_t presses = get_total_presses_count();

    switch (screen) {
        case _SCR_LEFT: 
            oled_clear();
            widget_kbd_lock();
            widget_current_layer(2);
            widget_left_encoder_layer(5);  
            widget_split_balance(9, &presses.left, PSTR("Left:"));
            widget_bongo_cat(13);
            break;
        case _SCR_RIGHT:
            oled_clear();
            if (menu_is_active()) {
                menu_render();
            } else {            
                widget_uptime(0);
                widget_avg_speed(4);
                widget_wpm(6);
                widget_split_balance(9, &presses.right, PSTR("Right:"));
                
                 // Keebart logo
                oled_blit_24x24_P(KEEBART_BITMAP_24x24, 20, 11);
                oled_set_cursor(2, 15);
                oled_write_P(PSTR("KEEBART"), false);
            }
            break;

        default:
            break;
    }
}

void widgets_add(void) {}
void widgets_remove(void) {}
