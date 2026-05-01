#include "quantum.h"
#include "oled_menu.h"
#include "_storage.h"
#include "_utils.h"
#include "print.h"
#include "transactions.h"
#include "_globals.h"
#include "rgb_matrix.h"

/*
    CUSTOM DECLARATIONS
*/
typedef struct {
    const char *name;
    uint8_t effect_id;
} rgb_map_t;

static const rgb_map_t rgb_items2 [] = {
    {"<- Back", -1},
    {"solid color", RGB_MATRIX_SOLID_COLOR },
    {"alphas mods", RGB_MATRIX_ALPHAS_MODS },
    {"gradient up-down", RGB_MATRIX_GRADIENT_UP_DOWN },
    {"gradient left-right", RGB_MATRIX_GRADIENT_LEFT_RIGHT },
    {"breathing", RGB_MATRIX_BREATHING },
    {"band sat", RGB_MATRIX_BAND_SAT },
    {"band val", RGB_MATRIX_BAND_VAL },
    {"band pinwheel sat", RGB_MATRIX_BAND_PINWHEEL_SAT },
    {"band pinwheel val", RGB_MATRIX_BAND_PINWHEEL_VAL },
    {"band spiral sat", RGB_MATRIX_BAND_SPIRAL_SAT },
    {"band spiral val", RGB_MATRIX_BAND_SPIRAL_VAL },
    {"cycle all", RGB_MATRIX_CYCLE_ALL },
    {"cycle left-right", RGB_MATRIX_CYCLE_LEFT_RIGHT },
    {"cycle up-down", RGB_MATRIX_CYCLE_UP_DOWN },
    {"cycle out-in", RGB_MATRIX_CYCLE_OUT_IN },
    {"cycle out-in dual", RGB_MATRIX_CYCLE_OUT_IN_DUAL },
    {"rainbow chevron", RGB_MATRIX_RAINBOW_MOVING_CHEVRON },
    {"cycle pinwheel", RGB_MATRIX_CYCLE_PINWHEEL },
    {"cycle spiral", RGB_MATRIX_CYCLE_SPIRAL },
    {"dual beacon", RGB_MATRIX_DUAL_BEACON },
    {"rainbow beacon", RGB_MATRIX_RAINBOW_BEACON },
    {"rainbow pinwheels", RGB_MATRIX_RAINBOW_PINWHEELS },
    // {"flower blooming", RGB_MATRIX_FLOWER_BLOOMING },
    {"raindrops", RGB_MATRIX_RAINDROPS },
    {"jellybean raindrops", RGB_MATRIX_JELLYBEAN_RAINDROPS },
    {"hue breathing", RGB_MATRIX_HUE_BREATHING },
    {"hue pendulum", RGB_MATRIX_HUE_PENDULUM },
    {"hue wave", RGB_MATRIX_HUE_WAVE },
    {"pixel fractal", RGB_MATRIX_PIXEL_FRACTAL },
    {"pixel flow", RGB_MATRIX_PIXEL_FLOW },
    {"pixel rain", RGB_MATRIX_PIXEL_RAIN },
    {"typing heatmap", RGB_MATRIX_TYPING_HEATMAP },
    {"digital rain", RGB_MATRIX_DIGITAL_RAIN },
    {"solid reactive simple", RGB_MATRIX_SOLID_REACTIVE_SIMPLE },
    {"solid reactive", RGB_MATRIX_SOLID_REACTIVE },
    {"solid reactive wide", RGB_MATRIX_SOLID_REACTIVE_WIDE },
    {"solid reactive multiwide", RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE },
    {"solid reactive cross", RGB_MATRIX_SOLID_REACTIVE_CROSS },
    {"solid reactive multicross", RGB_MATRIX_SOLID_REACTIVE_MULTICROSS },
    {"solid reactive nexus", RGB_MATRIX_SOLID_REACTIVE_NEXUS },
    {"solid reactive multinexus", RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS },
    {"splash", RGB_MATRIX_SPLASH },
    {"multisplash", RGB_MATRIX_MULTISPLASH },
    {"solid splash", RGB_MATRIX_SOLID_SPLASH },
    {"solid multisplash", RGB_MATRIX_CUSTOM_MULTISPLASH_BG } //RGB_MATRIX_SOLID_MULTISPLASH }
    // {"starlight", RGB_MATRIX_STARLIGHT },
    // {"starlight smooth", RGB_MATRIX_STARLIGHT_SMOOTH },
    // {"starlight dual hue", RGB_MATRIX_STARLIGHT_DUAL_HUE },
    // {"starlight dual sat", RGB_MATRIX_STARLIGHT_DUAL_SAT },
    // {"riverflow", RGB_MATRIX_RIVERFLOW },
    // {"effect max", RGB_MATRIX_EFFECT_MAX }
};

static menu_state_t g_state = MENU_OFF;
static uint8_t g_index = 0;
static uint32_t g_text_position_timer = 0;
static uint8_t g_text_first_idx = 0;

uint8_t vp_start = 0;
uint8_t vp_end = 15; //TODO: change VP_END in favor of global maxchars
uint8_t vp_index_incr = 0;
uint8_t last_anim = 0;

typedef struct {
    char* title;
    void (*action)(void);
    uint8_t negative_goto;
    uint8_t positive_goto;
} menu_dialog_item_t;

menu_dialog_item_t dialog_data; 
typedef union {
    uint32_t raw;
    struct {
        uint8_t rgb_mode;
        uint8_t left_anim;
        uint8_t right_anim;
        uint8_t reserved;
    };
} user_configx_t;

static user_configx_t g_config;

static uint8_t g_index_view_from = 0;
static uint32_t key_longpress_time;
static uint32_t menu_visible_time;

extern rgb_config_t rgb_matrix_config;

void update_viewport(void) {
    uint8_t by_how_much = 0;

    if (g_index > vp_end) {
        by_how_much = g_index - 15;
        vp_start = by_how_much;
        vp_end = 15 + by_how_much;
        if (vp_end > ARRAY_SIZE(rgb_items2)) {
            vp_end = ARRAY_SIZE(rgb_items2);
        }
        vp_index_incr = by_how_much;
        uprintf("g_index: %u, vp_start: %u, vp_end: %u, how_much: %u, vp_index_incr: %u \n", g_index, vp_start, vp_end, by_how_much, vp_index_incr);    
        return;
    } 

    //CURRENT_INDEX IS LOWER THAN THECURRENT VIEWPORT
    if (g_index < vp_start) {
        by_how_much = vp_start - g_index;

        vp_start -= by_how_much;
        vp_end -= by_how_much;

        vp_index_incr -= by_how_much;
        uprintf("g_index: %u, vp_start: %u, vp_end: %u, by_how_much: %u, vp_index_incr: %u \n", g_index, vp_start, vp_end, by_how_much, vp_index_incr);    
    }
}

void send_state_sync(void) {
    if(is_keyboard_master()) {
        menu_sync_m2s_t newstate = {g_state, g_index};
        (void)transaction_rpc_send(USER_SYNC_MENU, sizeof(newstate), &newstate);
    }
}

void send_encoder_sync(encoder_key_event_t event) {
    if(is_keyboard_master()) {
        (void)transaction_rpc_send(USER_SYNC_MENU_MOV, sizeof(event), &event);
    }
}

void menu_enter(void) {
    menu_visible_time = timer_read32();
    g_state = MENU_MAIN;
    g_index = 0;

    eeconfig_read_rgb_matrix(&rgb_matrix_config);
    last_anim = rgb_matrix_config.mode;

    send_state_sync();
}

void menu_exit(void) {
    if (g_state != MENU_OFF) {
        menu_visible_time = 0;
        g_state = MENU_OFF;

        send_state_sync();
    }
}

void save_eeprom(void) {
    rgb_matrix_sethsv(
        rgb_matrix_config.hsv.h,
        rgb_matrix_config.hsv.s,
        rgb_matrix_config.hsv.v
    );
    rgb_matrix_mode(
        rgb_matrix_config.mode
    );
    rgb_matrix_set_speed(rgb_matrix_config.speed);
    eeconfig_update_kb(rgb_matrix_config.raw);
}


uint8_t draw_item(const char* text, bool selected, uint8_t line, bool multiline) {
    uint8_t max_width = get_oled_limit('c');
    uint8_t text_length = strlen(text);
    char buf[11];
    oled_set_cursor(0, line++);

    if (selected) {
        if (timer_elapsed32(g_text_position_timer) > 400) {
            g_text_position_timer = timer_read32();
            if (g_text_first_idx + 10 < text_length) {
                g_text_first_idx += 1;
            }            
        }
        text += g_text_first_idx;
    }

    snprintf(buf, sizeof(buf), "%-*.*s", max_width, max_width, text);
    oled_write(buf, selected);

    if (multiline && !selected && text_length > max_width) {
        text += max_width;

        while (*text) {
            oled_set_cursor(0, line++);

            snprintf(buf, sizeof(buf), "%-*.*s", max_width, max_width, text);            
            oled_write(buf, selected); 
            text += max_width;
        }
    }

    return line;
}

uint8_t draw_line(const char* text, bool selected, uint8_t line) {
    return draw_item(text, selected, line, false);
}

void rotate_hsv(bool clockwise) {
    uint8_t max = 255;
    switch(g_index) {
        case 1:          
            if (clockwise) {
                if (rgb_matrix_config.hsv.h < max) {
                    rgb_matrix_config.hsv.h += 1;
                }
            } else if (rgb_matrix_config.hsv.h > 0) {
                rgb_matrix_config.hsv.h -= 1;
            }
            break;
        case 2: 
            if (clockwise) {
                if (rgb_matrix_config.hsv.s < max) {
                    rgb_matrix_config.hsv.s += 1;
                }
            } else if (rgb_matrix_config.hsv.s > 0) {
                rgb_matrix_config.hsv.s -= 1;
            }
            break;
        case 3: 
            max = 101;
            if (clockwise) {
                if (rgb_matrix_config.hsv.v < max) {
                    rgb_matrix_config.hsv.v += 1;
                }
            } else if (rgb_matrix_config.hsv.v > 0) {
                rgb_matrix_config.hsv.v -= 1;
            }
            break;
    }
    rgb_matrix_sethsv_noeeprom(rgb_matrix_config.hsv.h, rgb_matrix_config.hsv.s, rgb_matrix_config.hsv.v);
    send_encoder_sync(clockwise ? ENC_CLK : ENC_CNTCLK);
}

/*
    END CUSTOM DECLARATIONS
*/

void menu_init(void) {    
    g_state = MENU_OFF;
    g_index = 0;    
}

bool menu_is_active(void) {
    return g_state != MENU_OFF;
}

void menu_encoder_rotate(bool clockwise) {
    uint8_t max = 0;
    menu_visible_time = timer_read32();

    switch (g_state) {
        case MENU_MAIN: max = 5; break;
        case MENU_RGB: max = 4; break;
        case MENU_RGB_ANIM: max = ARRAY_SIZE(rgb_items2); break;
        case MENU_RGB_COLOR: max = 4; break;
        case MENU_RGB_COLOR_DIAL: 
            rotate_hsv(clockwise);                 
            return;
        case MENU_RGB_SPEED_DIAL:
            max = 255; 
            if (clockwise) {
                if (rgb_matrix_config.speed < max) {
                    rgb_matrix_config.speed += 1;
                }
            } else if (rgb_matrix_config.speed > 0) {
                rgb_matrix_config.speed -= 1;
            }
            rgb_matrix_set_speed_noeeprom(rgb_matrix_config.speed);
            send_encoder_sync(clockwise ? ENC_CLK : ENC_CNTCLK);
            return;
        case MENU_DIALOG: max = 2; break;
        case MENU_LEFT_ANIM: max = 3; break;
        case MENU_RIGHT_ANIM: max = 2; break;
        default: return;
    }    

    if (clockwise) {       
        if ((g_index + 1) >= max) {
            g_index = 0;
        } else {
            g_index++;
        }
    } else {
        if (g_index == 0) { 
            g_index = max -1;
        } else {
            g_index = (g_index - 1);
        }
    }

    if (g_state == MENU_RGB_ANIM) {
        if (g_index == 0) {
            rgb_matrix_config.mode = last_anim;
        } else {
            rgb_matrix_config.mode = rgb_items2[g_index].effect_id;
        }
        rgb_matrix_mode_noeeprom(rgb_matrix_config.mode);        
    }

    update_viewport();
    g_text_position_timer = timer_read32();
    g_text_first_idx = 0;

    send_encoder_sync(clockwise ? ENC_CLK : ENC_CNTCLK);
}

void menu_encoder_press(void) {
   
    switch (g_state) {
        case MENU_MAIN:
            switch (g_index) {
                case 0: menu_exit(); break;
                case 1: g_state = MENU_RGB; break;
                case 2: g_state = MENU_LEFT_ANIM; break;
                case 3: g_state = MENU_RIGHT_ANIM; break;
                case 4: 
                    dialog_data = (menu_dialog_item_t) {
                        "Save cfg. to EEPROM?\0",
                        save_eeprom,
                        MENU_MAIN, 
                        MENU_MAIN
                    };
                    g_state = MENU_DIALOG;                
                    break;
            }
            g_index = 0;
            break;

        case MENU_RGB:
            switch (g_index) {
                case 0: g_state = MENU_MAIN; break;
                case 1: g_state = MENU_RGB_ANIM; break;
                case 2: g_state = MENU_RGB_COLOR; break;
                case 3: g_state = MENU_RGB_SPEED_DIAL; break;                
            }
            g_index = 0;
            break;

        case MENU_RGB_ANIM:
            if (g_index == 0) {
                //revert to last animation if i selected back
                g_state = MENU_RGB;
                rgb_matrix_mode_noeeprom(last_anim);
            } else {                
                rgb_matrix_config.mode = rgb_items2[g_index].effect_id;
                last_anim = rgb_matrix_config.mode;
                g_index = 0;
                g_state = MENU_RGB;
            }
            break;

        case MENU_RGB_COLOR:
            if (g_index == 0) {
                g_state = MENU_RGB;
            } else {
                g_state = MENU_RGB_COLOR_DIAL;
                rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
            }
            //DO NOT reset the g_index because 
            //i want to know what values im changing in the color dial
            break;
        case MENU_RGB_SPEED_DIAL: 
            g_index = 0;
            g_state = MENU_RGB;
            break;

        case MENU_RGB_COLOR_DIAL: 
            //reset the animation and go back
            rgb_matrix_mode_noeeprom(last_anim);
            g_state = MENU_RGB_COLOR;
            g_index = 0;
            break;

        case MENU_LEFT_ANIM:
            if (g_index == 0) {
                g_state = MENU_MAIN;                
            } else {
                g_config.left_anim = g_index - 1;
            }
            break;

        case MENU_RIGHT_ANIM:
            if (g_index == 0) {
                g_state = MENU_MAIN;
            } else {
                g_config.right_anim = g_index - 1;
            }
            break;

        case MENU_DIALOG: 
            if (g_index == 0) {
                //go back to where i was;
                g_state = dialog_data.negative_goto;
            } else if (g_index == 1) {
                //call function pointer and return to previous menu
                dialog_data.action();
                g_state = dialog_data.positive_goto;
                g_index = 0;
            }   
            break;
        default:
            break;
    }
    g_index_view_from = 0;

    send_encoder_sync(ENC_PRESS);
}

void menu_render(void) {
    uint8_t w = get_oled_limit('c');

    char buf[w + 1];
    uint8_t line = 0;

    if (w > (int)sizeof(buf) - 1) {
        w = sizeof(buf) - 1;
    }
    
    oled_clear();
    switch (g_state) {
        case MENU_MAIN:
            line = draw_line("<- Exit", g_index == 0, line);
            line = draw_line("RGB", g_index == 1, line);
            line = draw_line("L-Scr", g_index == 2, line);
            line = draw_line("R-Scr", g_index == 3, line);
            draw_line("* Save", g_index == 4, 15);
            break;

        case MENU_RGB: 
            line = draw_line("<- Back", g_index == 0, line);
            line = draw_line("Animation", g_index == 1, line);
            line = draw_line("Color", g_index == 2, line);
            draw_line("Speed", g_index == 3, line);
            break;
        case MENU_RGB_ANIM:
            for (uint8_t i = vp_start, j = 0; i <= vp_end; i++, j++) {                
                draw_line(rgb_items2[i].name, i == g_index, j);
            }
            break;

        case MENU_RGB_COLOR:
            line = draw_line("<- Back", g_index == 0, line);
            line = draw_line("Hue", g_index == 1, line);
            line = draw_line("Sat.", g_index == 2, line);
            draw_line("Bright.", g_index == 3, line);
            break;
        case MENU_RGB_SPEED_DIAL: 
            line = draw_line("Anim. spd:", false, line);
            snprintf(buf, sizeof(buf), "%*u",  w, rgb_matrix_config.speed);
            draw_line(buf, false, line);
            break;
        case MENU_RGB_COLOR_DIAL:
            if (g_index == 1) {
                line = draw_line("Hue:", false, line);
                snprintf(buf, sizeof(buf), "%*u",  w, rgb_matrix_config.hsv.h);
            } else if (g_index == 2) {
                line = draw_line("Sat.:", false, line);
                snprintf(buf, sizeof(buf), "%*u",  w, rgb_matrix_config.hsv.s);
            } else if (g_index == 3) {
                line = draw_line("Bright.:", false, line);
                snprintf(buf, sizeof(buf), "%*u",  w, rgb_matrix_config.hsv.v);
            } else {
                line = draw_line("Out of scope", false, line);
                return;
            }

            draw_line(buf, false, line);
            break;

        case MENU_LEFT_ANIM:
            line = draw_line("<- Back", g_index == 0, line);
            line = draw_line("None", g_index == 1, line);
            draw_line("BongoCat", g_index == 2, line);
            break;

        case MENU_RIGHT_ANIM:
            line = draw_line("<- Back", g_index == 0, line);
            draw_line("None", g_index == 1, line);
            break;
        
        case MENU_DIALOG: 
            draw_item(dialog_data.title, false, line, true);
            draw_line("No", g_index == 0, 6);
            draw_line("Yes", g_index == 1, 7);
            break;

        default:
            break;
    }
}


bool menu_check_keypress(uint16_t keycode, bool pressed){
    //results true means that this function has handled the the keypress
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
        if (timer_elapsed32(menu_visible_time) > 40000) {
            //close menu
            menu_exit();
        }
    }
}


void menu_handle_state_from_remote(uint8_t in_len, const void* in_data) {
    if (in_len >= sizeof(menu_state_t)) {

        menu_sync_m2s_t newstate = {0,0};
        memcpy(&newstate, in_data, sizeof(menu_sync_m2s_t));

        g_state = newstate.state;
        g_index = newstate.index;
    }
}

void menu_handle_mov_from_remote(uint8_t in_len, const void* in_data) {
    if (in_len >= sizeof(encoder_key_event_t)) {

        encoder_key_event_t newstate = {255};
        memcpy(&newstate, in_data, sizeof(encoder_key_event_t));

        switch (newstate) {
            case ENC_CLK: 
                menu_encoder_rotate(true);
                break;
            case ENC_CNTCLK:
                menu_encoder_rotate(false);
                break;
            case ENC_PRESS:
                menu_encoder_press();
                break;
        }
    }
}

