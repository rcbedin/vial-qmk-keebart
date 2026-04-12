#include "quantum.h"
#include "oled_menu.h"
#include "_storage.h"
#include "_utils.h"
#include "print.h"

/*
    CUSTOM DECLARATIONS
*/

static const char *rgb_items[] = {
    "Back",
    "alphas_mods",
    "gradient_up_down",
    "gradient_left_right",
    "breathing",
    "band_sat",
    "band_val",
    "band_pinwheel_sat",
    "band_pinwheel_val",
    "band_spiral_sat",
    "band_spiral_val",
    "cycle_all",
    "cycle_left_right",
    "cycle_up_down",
    "rainbow_moving_chevron",
    "cycle_out_in",
    "cycle_out_in_dual",
    "cycle_pinwheel",
    "cycle_spiral",
    "dual_beacon",
    "rainbow_beacon",
    "rainbow_pinwheels",
    "raindrops",
    "jellybean_raindrops",
    "hue_breathing",
    "hue_pendulum",
    "hue_wave",
    "pixel_fractal",
    "pixel_flow",
    "pixel_rain",
    "typing_heatmap",
    "digital_rain",    
    "solid_reactive_simple",
    "solid_reactive",
    "solid_reactive_wide",
    "solid_reactive_multiwide",
    "solid_reactive_cross",
    "solid_reactive_multicross",
    "solid_reactive_nexus",
    "solid_reactive_multinexus",
    "splash",
    "multisplash",
    "solid_splash",
    "solid_multisplash",
};



static uint8_t g_index = 0;
static uint32_t g_text_position_timer = 0;
static uint8_t g_text_first_idx = 0;

uint8_t vp_start = 0;
uint8_t vp_end = 15;
uint8_t vp_index_incr = 0;

void update_viewport(void) {
    uint8_t by_how_much = 0;

    if (g_index > vp_end) {
        by_how_much = g_index - 15;
        vp_start = by_how_much;
        vp_end = 15 + by_how_much;
        if (vp_end > ARRAY_SIZE(rgb_items)) {
            vp_end = ARRAY_SIZE(rgb_items);
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
/*
    END CUSTOM DECLARATIONS
*/

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


static uint8_t g_index_view_from = 0;

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
        case MENU_RGB: max = ARRAY_SIZE(rgb_items); break;
        case MENU_LEFT_ANIM: max = 5; break;
        case MENU_RIGHT_ANIM: max = 5; break;
        default: return;
    }    

    if (clockwise) {
        uprintf("g_index: %u, max: %u\n", g_index, max);    
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

    update_viewport();
    g_text_position_timer = timer_read32();
    g_text_first_idx = 0;
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
            g_index_view_from = 0; 
            break;

        case MENU_RGB:
            if (g_index == 0) {
                g_state = MENU_MAIN;
                g_index_view_from = 0; 
            } else {
                g_config.rgb_mode = g_index - 1;
                storage_save();
            }
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
            // char buf[9]; 
            for (uint8_t i = vp_start, j = 0; i <= vp_end; i++, j++) {
                oled_set_cursor(0, j);

                draw_item(rgb_items[i], i == g_index);
            }
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
    uint8_t text_length = strlen(text); 
    
    if (strcmp(text, "breathing") == 0) {
        uprintf("text length is %u", text_length);
    }

    char buf[9]; 

    if (selected) {
        oled_write_P(PSTR("> "), false);
        if (timer_elapsed32(g_text_position_timer) > 400) {
            g_text_position_timer = timer_read32();
            if (g_text_first_idx + 8 < text_length) {
                g_text_first_idx += 1;
            }             
        }
        strncpy(buf, text + g_text_first_idx, 8);

    } else {
        oled_write_P(PSTR("  "), false);
        strncpy(buf, text, 8);
    }

    buf[8] = '\0';
    oled_write(buf, false);
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
        if (timer_elapsed32(menu_visible_time) > 40000) {
            //close menu
            menu_exit();        
        }
    }
}
