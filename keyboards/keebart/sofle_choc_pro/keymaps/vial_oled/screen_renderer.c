#include "quantum.h"
#include "screen_renderer.h"
#include "eeconfig.h"
#include "bitmaps.h"
#include "_storage.h"
#include "_utils.h"
#include "_globals.h"
#include "animations/bongo_cat.h"
#include "animations/bongo_cat_bitmaps.h"

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
    oled_print_right_aligned(buf, g_oled_max_char);

    return start_row;
}

uint8_t widget_avg_speed(uint8_t start_row) {
    oled_set_cursor(0, start_row++);
    oled_write_P(PSTR("Avg Speed"), false);
    oled_set_cursor(0, start_row++);
    oled_write_P(PSTR("(25 s):"), false);

    // const uint8_t wpm      = get_current_wpm();
    uint16_t      wpm      = (uint16_t)get_current_wpm() * 10u;
    uint16_t      wpm_int  = wpm / 10u;
    uint16_t      wpm_frac = wpm % 10u;

    // TODO: consider remove snprintf
    char buf[11];
    snprintf(buf, sizeof(buf), "%3u.%1u WPM", wpm_int, wpm_frac);
    oled_set_cursor(0, start_row++);
    oled_print_right_aligned(buf, g_oled_max_char);

    return start_row;
}

uint8_t widget_wpm(uint8_t start_row) {
    uint16_t wpm      = get_current_dwpm();
    uint16_t wpm_int  = wpm / 10u;
    uint16_t wpm_frac = wpm % 10u;

    // TODO: consider remove snprintf
    char buf[11];
    snprintf(buf, sizeof(buf), "%3u.%1u WPM", wpm_int, wpm_frac);
    oled_set_cursor(0, start_row++);
    oled_print_right_aligned(buf, g_oled_max_char);

    return start_row;
}

uint8_t widget_split_balance(uint8_t start_row, const uint32_t *presses_qt, const char *side) {   
    uint32_t total = local_presses_left + local_presses_right;
    if (total == 0) {
        total = 1;  // avoid div by 0
    }

    uint8_t pct_calc = round_percentage((100.0f * (*presses_qt)) / total);

    oled_set_cursor(0, start_row++);
    oled_write_P(side, false);

    char buf[6];
    snprintf(buf, sizeof(buf), "%3u %%", pct_calc);
    oled_set_cursor(0, start_row++);
    oled_print_right_aligned(buf, g_oled_max_char);

    return start_row;
}

uint8_t widget_side_press_percentage(void) {
     // if (is_keyboard_master()) {
    //     local_presses_left = g_press_left;
    //     local_presses_right = g_press_right;
    // } else {
    //     local_presses_left = g_remote_presses.left;
    //     local_presses_right = g_remote_presses.right;
    // }


    
    // uint32_t total = local_presses_left + local_presses_right;
    // if (total == 0) {
    //     total = 1;  // avoid div by 0
    // }
    // uint8_t pct_left = round_percentage((100.0f * local_presses_left) / total);
    // uint8_t pct_right = round_percentage((100.0f * local_presses_right) / total);
    return 0;
}


uint8_t widget_bongo_cat(uint8_t start_row) {
    oled_set_cursor(0,start_row++);
    oled_write_P(BONGO_R1, false);

    for(int i = 0; i < 2; i++) {
        const char *frame = (const char *)pgm_read_ptr(&current_bongo_anim_frames[i]);
        oled_set_cursor(0, start_row + i);
        oled_write_P(frame, false);
    }

    return start_row ;
}


void widgets_init(void) {}

void widgets_render(e_oled_screen screen) {
    oled_clear();

    switch (screen) {
        case _SCR_LEFT:
            widget_kbd_lock();
            widget_current_layer(2);

            widget_bongo_cat(13);
            break;
        case _SCR_RIGHT:
        
            uint8_t curr_row = 0;
            curr_row = widget_uptime(curr_row);
            curr_row = widget_avg_speed(curr_row);
            curr_row = widget_wpm(curr_row);
            curr_row = widget_split_balance(curr_row, &local_presses_right, PSTR("Right:"));
            break;

        default:
            break;
    }

    // uint8_t curr_row = 0;

    // oled_set_cursor(0, curr_row);
    // oled_write_P(PSTR("EncLayer:"), false);

    // oled_set_cursor(0, ++curr_row);

    //  if (l_enc_layer == _EC_L_OLED_SPECIAL) {
    //     oled_print_right_aligned(PSTR("SPECIAL"), g_oled_max_char);
    // } else {
    //     char buf[4];
    //     snprintf(buf, sizeof(buf), "%1u", l_enc_layer);
    //     oled_print_right_aligned(buf, g_oled_max_char);
    // }

    // render_split_balance(&local_presses_left, PSTR("Left:"), &total);
}

void widgets_add(void) {}
void widgets_remove(void) {}
