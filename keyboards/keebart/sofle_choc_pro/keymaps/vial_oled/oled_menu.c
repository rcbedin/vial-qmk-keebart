#include "quantum.h"
#include "oled_menu.h"
#include "_storage.h"
#include "_utils.h"

typedef enum {
    MENU_OFF,
    MENU_MAIN,
    MENU_RGB,
    MENU_LEFT_ANIM,
    MENU_RIGHT_ANIM
} menu_state_t;

typedef union {
    uint32_t raw;
    struct {
        uint8_t rgb_mode;
        uint8_t left_anim;
        uint8_t right_anim;
        uint8_t reserved;
    };
} user_config_t;

static user_config_t g_config;
static menu_state_t g_state = MENU_OFF;
static uint8_t g_index = 0;

static uint32_t key_longpress_time;
static uint32_t menu_visible_time;

void menu_init(void) {    
    g_state = MENU_OFF;
    g_index = 0;
}

void menu_enter(void) {
    menu_visible_time = timer_read32();
    g_state = MENU_MAIN;
    g_index = 0;
}

void menu_exit(void) {
    if (g_state != MENU_OFF) {
        menu_visible_time = 0;
        g_state = MENU_OFF;
    }
}

bool menu_is_active(void) {
    return g_state != MENU_OFF;
}

void menu_encoder_rotate(bool clockwise) {
    uint8_t max = 0;
    menu_visible_time = timer_read32();

    switch (g_state) {
        case MENU_MAIN: max = 4; break;
        case MENU_RGB: max = 6; break;
        case MENU_LEFT_ANIM: max = 5; break;
        case MENU_RIGHT_ANIM: max = 5; break;
        default: return;
    }

    if (clockwise) {
        g_index = (g_index + 1) % max;
    } else {
        g_index = (g_index == 0) ? (max - 1) : (g_index - 1);
    }
}

void menu_encoder_press(void) {
    switch (g_state) {

        case MENU_MAIN:
            switch (g_index) {
                case 0: menu_exit(); break;
                case 1: g_state = MENU_RGB; break;
                case 2: g_state = MENU_LEFT_ANIM; break;
                case 3: g_state = MENU_RIGHT_ANIM; break;
            }
            g_index = 0;
            break;

        case MENU_RGB:
            if (g_index == 0) {
                g_state = MENU_MAIN;
            } else {
                g_config.rgb_mode = g_index - 1;
                storage_save();
            }
            break;

        case MENU_LEFT_ANIM:
            if (g_index == 0) {
                g_state = MENU_MAIN;
            } else {
                g_config.left_anim = g_index - 1;
                storage_save();
            }
            break;

        case MENU_RIGHT_ANIM:
            if (g_index == 0) {
                g_state = MENU_MAIN;
            } else {
                g_config.right_anim = g_index - 1;
                storage_save();
            }
            break;

        default:
            break;
    }
}

void menu_render(void) {

    oled_clear();

    switch (g_state) {

        case MENU_MAIN:
            draw_item("Exit", g_index == 0);
            oled_set_cursor(0,1);
            draw_item("RGB", g_index == 1);
            oled_set_cursor(0,2);
            draw_item("L-Scr", g_index == 2);
            oled_set_cursor(0,3);
            draw_item("R-Scr", g_index == 3);
            break;

        case MENU_RGB:
            draw_item("Back", g_index == 0);
            oled_set_cursor(0,1);
            draw_item("Static", g_index == 1);
            oled_set_cursor(0,2);
            draw_item("Breathing", g_index == 2);
            oled_set_cursor(0,3);
            draw_item("Rainbow", g_index == 3);
            break;

        case MENU_LEFT_ANIM:
            draw_item("Back", g_index == 0);
            oled_set_cursor(0,1);
            draw_item("None", g_index == 1);
            oled_set_cursor(0,2);
            draw_item("BongoCat", g_index == 2);
            break;

        case MENU_RIGHT_ANIM:
            draw_item("Back", g_index == 0);
            oled_set_cursor(0,1);
            draw_item("None", g_index == 1);
            // oled_set_cursor(0,2);
            // draw_item("Anim2", g_index == 2);
            break;

        default:
            break;
    }
}

void draw_item(const char* text, bool selected) {
    if (selected) {
        oled_write_P(PSTR("> "), false);
    } else {
        oled_write_P(PSTR("  "), false);
    }
    oled_write(text, false);
}


bool menu_check_keypress(uint16_t keycode, bool pressed){
    //results true means that this function has handled the function
    if (keycode == KC_F14) {
        if (pressed) {
            if (menu_is_active()) {
                menu_encoder_press();        
            } else {
                if (key_longpress_time == 0) {
                    key_longpress_time = timer_read32();
                }  
            }
        } else {
            key_longpress_time = 0;
        }
        return true;
    }

    return false;
}

void menu_check_usertime(void) {
    if (key_longpress_time > 0) {
        const uint32_t elapsed_press = timer_elapsed32(key_longpress_time);       
        if (elapsed_press > 1000) {
            menu_enter();
            return;
        }
    }
    
    if (menu_visible_time > 0 && key_longpress_time == 0){
        if (timer_elapsed32(menu_visible_time) > 10000) {
            //close menu
            menu_exit();        
        }
    }
}
