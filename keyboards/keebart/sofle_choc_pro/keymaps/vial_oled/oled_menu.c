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
    // {"solid color", RGB_MATRIX_SOLID_COLOR },
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
    {"solid multisplash", RGB_MATRIX_SOLID_MULTISPLASH }
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

int16_t hsv[3];

uint8_t vp_start = 0;
uint8_t vp_end = 15; //TODO: change VP_END in favor of global maxchars
uint8_t vp_index_incr = 0;

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
    eeconfig_update_kb(rgb_matrix_config.raw);
}

void draw_item(const char* text, bool selected) {
    uint8_t text_length = strlen(text);
    char buf[11]; 

    if (selected) {
        if (timer_elapsed32(g_text_position_timer) > 400) {
            g_text_position_timer = timer_read32();
            if (g_text_first_idx + 10 < text_length) {
                g_text_first_idx += 1;
            }             
        }
        snprintf(buf, sizeof(buf), "%-10.10s", text + g_text_first_idx);

    } else {
        snprintf(buf, sizeof(buf), "%-10.10s", text);
    }

    oled_write(buf, selected);
}


/*
    END CUSTOM DECLARATIONS
*/

void menu_init(void) {    
    g_state = MENU_OFF;
    g_index = 0;
    eeconfig_read_rgb_matrix(&rgb_matrix_config);

    hsv[0] = rgb_matrix_config.hsv.h;
    hsv[1] = rgb_matrix_config.hsv.s;
    hsv[2] = rgb_matrix_config.hsv.v;
}

bool menu_is_active(void) {
    return g_state != MENU_OFF;
}

void menu_encoder_rotate(bool clockwise) {
    uint8_t max = 0;
    menu_visible_time = timer_read32();

    switch (g_state) {
        case MENU_MAIN: max = 5; break;
        case MENU_RGB: max = 3; break;
        case MENU_RGB_ANIM: max = ARRAY_SIZE(rgb_items2); break;
        case MENU_RGB_COLOR: max = 4; break;
        case MENU_RGB_COLOR_DIAL: max = 1; break;
        case MENU_LEFT_ANIM: max = 5; break;
        case MENU_RIGHT_ANIM: max = 5; break;
        default: return;
    }    

    if (clockwise) {
        if (g_state == MENU_RGB_COLOR_DIAL) {
            if (hsv[g_index - 1] < 255) {
                hsv[g_index - 1] += 1;
            }
        } else {
            if ((g_index + 1) >= max) {
                g_index = 0;
            } else {
                g_index++;
            }
        }
    } else {
        if (g_state == MENU_RGB_COLOR_DIAL) {
            //TODO: change color value
            if (hsv[g_index - 1] > 0) {
                hsv[g_index - 1] -= 1;
            }
        } else {
            if (g_index == 0) { 
                g_index = max -1;
            } else {
                g_index = (g_index - 1);
            }
        }
    }

    if (g_state == MENU_RGB_ANIM) {
        rgb_matrix_mode(rgb_items2[g_index].effect_id);
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
                case 4: save_eeprom(); break;
            }
            g_index = 0;
            g_index_view_from = 0; 
            break;

        case MENU_RGB:
            switch (g_index) {
                case 0: g_state = MENU_MAIN; break;
                case 1: g_state = MENU_RGB_ANIM; break;
                case 2: g_state = MENU_RGB_COLOR; break;
            }
            g_index = 0;
            g_index_view_from = 0;
            break;

        case MENU_RGB_ANIM:
            if (g_index == 0) {
                g_state = MENU_RGB;
                g_index_view_from = 0; 
            } else {
                rgb_matrix_mode_noeeprom(rgb_items2[g_index].effect_id);              
            }
            break;

        case MENU_RGB_COLOR:
            if (g_index == 0) {
                g_state = MENU_RGB;
                g_index_view_from = 0; 
            } else {
                g_state = MENU_RGB_COLOR_DIAL;
            }
            //DO NOT reset the g_index because 
            //i want to know what values im changing in the color dial
            break;

        case MENU_RGB_COLOR_DIAL: 
            //go back without asking
            g_state = MENU_RGB_COLOR;
            g_index = 0;
            g_index_view_from = 0;
            break;

        case MENU_LEFT_ANIM:
            if (g_index == 0) {
                g_state = MENU_MAIN;                
                g_index_view_from = 0;  
            } else {
                g_config.left_anim = g_index - 1;
                storage_save();
            }
            break;

        case MENU_RIGHT_ANIM:
            if (g_index == 0) {
                g_state = MENU_MAIN;
                g_index_view_from = 0;  
            } else {
                g_config.right_anim = g_index - 1;
                storage_save();
            }
            break;

        default:
            break;
    }

    send_encoder_sync(ENC_PRESS);
}


void menu_render(void) {
    uint8_t max_width = get_oled_limit('c');
    char buf[max_width + 1];

    int w = max_width;
    if (w > (int)sizeof(buf) - 1) {
        w = sizeof(buf) - 1;
    }

    
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
            oled_set_cursor(0,15);
            draw_item("Save EEPROM", g_index == 4);
            break;

        case MENU_RGB: 
            draw_item("<- Back", g_index == 0);
            oled_set_cursor(0,1);
            draw_item("Animation", g_index == 1);
            oled_set_cursor(0,2);
            draw_item("Color", g_index == 2);
            oled_set_cursor(0,3);

            break;
        case MENU_RGB_ANIM:
            // char buf[9]; 
            for (uint8_t i = vp_start, j = 0; i <= vp_end; i++, j++) {
                oled_set_cursor(0, j);
                draw_item(rgb_items2[i].name, i == g_index);
            }
            break;

        case MENU_RGB_COLOR:
            draw_item("<- Back", g_index == 0);
            oled_set_cursor(0,1);
            draw_item("Hue", g_index == 1);

            oled_set_cursor(0,2);
            draw_item("Saturation", g_index == 2);
            
            oled_set_cursor(0,3);
            draw_item("Value", g_index == 3);

            break;
            
        case MENU_RGB_COLOR_DIAL:
            if (g_index == 1) {
                //HUE 
                draw_item("Hue:", false);
                oled_set_cursor(0,1);                    
                snprintf(buf, sizeof(buf), "%*u",  w, hsv[g_index - 1]);
                draw_item(buf, false);
                oled_set_cursor(0,3);
                draw_item("square", false);
            } else if (g_index == 2) {
                //Saturation
                draw_item("Sat.:", false);
                oled_set_cursor(0,1);
                snprintf(buf, sizeof(buf), "%*u",  w, hsv[g_index - 1]); //rgb_matrix_config.hsv.s);
                draw_item(buf, false);
                oled_set_cursor(0,3);
                draw_item("square", false);
            } else if (g_index == 3) {
                //Value | brightness
                draw_item("Value:", false);
                oled_set_cursor(0,1);
                snprintf(buf, sizeof(buf), "%*u",  w, hsv[g_index - 1] );// rgb_matrix_config.hsv.v);
                draw_item(buf, false);
                oled_set_cursor(0,3);
                draw_item("square", false); 
            } else {
                draw_item("Out of scope", false);
            }
            break;

        case MENU_LEFT_ANIM:
            draw_item("<- Back", g_index == 0);
            oled_set_cursor(0,1);
            draw_item("None", g_index == 1);
            oled_set_cursor(0,2);
            draw_item("BongoCat", g_index == 2);
            break;

        case MENU_RIGHT_ANIM:
            draw_item("<- Back", g_index == 0);
            oled_set_cursor(0,1);
            draw_item("None", g_index == 1);
            // oled_set_cursor(0,2);
            // draw_item("Anim2", g_index == 2);
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

