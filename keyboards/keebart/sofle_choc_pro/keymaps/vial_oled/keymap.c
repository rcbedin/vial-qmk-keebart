#include QMK_KEYBOARD_H

#ifdef OLED_ENABLE
#include "bitmaps.h"
#include "_storage.h"
#include "_utils.h"
#include "_globals.h"
#include "animations/bongo_cat.h"
// #include "roboeyes.h"
#include "oled_menu.h"
#include "screen_renderer.h"
#include "print.h"

#endif

enum layers {
    _BASE  = 0,
    _LOWER = 1,
    _RAISE = 2
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*
 * QWERTY
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |  ESC |   1  |   2  |   3  |   4  |   5  |                    |   6  |  7   |   8  |   9  |   0  | Bspc |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |  Tab |   Q  |   W  |   E  |   R  |   T  |                    |   Y  |   U  |   I  |   O  |   P  |   \| |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |  lsh |   A  |   S  |   D  |   F  |   G  |-------.    ,-------|   H  |   J  |   K  |   L  |   ;  |  '"  |
 * |------+------+------+------+------+------|  F13  |    | F14   |------+------+------+------+------+------|
 * | LCTR |   Z  |   X  |   C  |   V  |   B  |-------|    |-------|   N  |   M  |   ,  |   .  |   /  | `    |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *            | mins | win  |LAlt  |LOWER | /Space  /       \Enter \  |RAISE | =+   | [    |     ] |           
 *            |      |      |      |      |/       /         \      \ |      |      |      |      |
 *            `----------------------------------'           '------''---------------------------'
 */

[_BASE] = LAYOUT_split_4x6_5(
  KC_ESC,   KC_1,  KC_2,  KC_3,  KC_4,  KC_5,                        KC_6,    KC_7,    KC_8,    KC_9,    KC_0,     KC_BSPC,
  KC_TAB,   KC_Q,  KC_W,  KC_E,  KC_R,  KC_T,                        KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,     KC_BACKSLASH,
  KC_LSFT,  KC_A,  KC_S,  KC_D,  KC_F,  KC_G,                        KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN,  KC_QUOT,
  KC_LCTL,  KC_Z,  KC_X,  KC_C,  KC_V,  KC_B,   KC_F13,     KC_F14,  KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,  KC_MINS,
             KC_GRV, KC_LWIN, TL_LOWR, KC_LALT, KC_SPC,     KC_ENT,  KC_EQL,  TL_UPPR, KC_LBRC, KC_RBRC
),
/* LOWER
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * | esc  |  F1  |  F2  |  F3  |  F4  |  F5  |                    |   F6 |  F7  |  f8  |  f9  | f10  | Bspc |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | tab  |  f11 |  f12 |      |      |      |                    |      |      |  up  |      |      |  del |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | shift|      |      | pgup |c+s+up| home |-------.    ,-------| c+s+u| left | dwn  |right |      |      |
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * | lctrl|      |      | pgdw |c+s+dw| end  |-------|    |-------|c+s+d |      | ct+le| ct+ri|      |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *            |      |      | LOWER|      | /       /       \      \  |      |      |      |      |
 *            |      |      |      |      |/       /         \      \ |      |      |      |      |
 *            `----------------------------------'           '------''---------------------------'
 */   
[_LOWER] = LAYOUT_split_4x6_5(
  KC_ESC,   KC_F1,   KC_F2,   KC_F3,   KC_F4,         KC_F5,                           KC_F6,         KC_F7,    KC_F8,       KC_F9,       KC_F10,   KC_BSPC,
  KC_TAB,   KC_F11,  KC_F12,  XXXXXXX, XXXXXXX,       XXXXXXX,                         XXXXXXX,       XXXXXXX,  KC_UP,       XXXXXXX,     XXXXXXX,  KC_DELETE,
  KC_LSFT,  XXXXXXX, XXXXXXX, KC_PGUP, XXXXXXX,       KC_HOME,                         C(S(KC_UP)),   KC_LEFT,  KC_DOWN,     KC_RGHT,     XXXXXXX,  XXXXXXX,
  KC_LCTL,  XXXXXXX, XXXXXXX, KC_PGDN, XXXXXXX,       KC_END,  _______,      _______,  C(S(KC_DOWN)), XXXXXXX,  C(KC_LEFT),  C(KC_RIGHT), XXXXXXX,  XXXXXXX, 
                XXXXXXX,  KC_LWIN, _______, KC_LALT,  KC_SPC,                          KC_ENT, KC_EQL, _______, KC_LBRC, KC_RBRC
),
/* RAISE
 * ,----------------------------------------.                    ,-----------------------------------------.
 * |  esc |w+s+l |w+s+r | w+l  | w+r  | w+u  |                    |  *   |  7   |  8   |  9   |      | bks  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |  tab |      |      | up   |      | w+d  |                    |  /   |  4   |  5   |  6   |      | del  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | shift|      | left | dwn  | right|      |-------.    ,-------|  +   |  1   |  2   |  3   |      | ins  |
 * |------+------+------+------+------+------|        |   |       |------+------+------+------+------+------|
 * |  ctl |      |      |      |      | print|-------|    |-------|  -   |      |  0   |  .   |      |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *            |      |      |      |      | /       /       \      \  |      | RAISE|  num |  caps|
 *            |      |      |      |      |/       /         \      \ |      |      |      |      |
 *            `----------------------------------'           '------''---------------------------'
 */
[_RAISE] = LAYOUT_split_4x6_5(
  KC_ESC,  G(S(KC_LEFT)), G(S(KC_RIGHT)), G(KC_LEFT), G(KC_RIGHT), G(KC_UP),                             KC_KP_ASTERISK, KC_KP_7,   KC_KP_8,   KC_KP_9,   XXXXXXX, KC_BSPC,
  KC_TAB,  XXXXXXX,       XXXXXXX,        KC_UP,      XXXXXXX,     G(KC_DOWN),                           KC_KP_SLASH,    KC_KP_4,   KC_KP_5,   KC_KP_6,   XXXXXXX, KC_DELETE,
  KC_LSFT, XXXXXXX,       KC_LEFT,        KC_DOWN,    KC_RIGHT,    XXXXXXX,                              KC_KP_PLUS,     KC_KP_1,   KC_KP_2,   KC_KP_3,   XXXXXXX, KC_INSERT,
  KC_LCTL, XXXXXXX,       XXXXXXX,        XXXXXXX,    XXXXXXX,     KC_PSCR,  _______,          _______,  KC_KP_MINUS,    XXXXXXX,   KC_KP_0,   KC_KP_DOT, XXXXXXX, XXXXXXX,
                       XXXXXXX, KC_LWIN, _______, KC_LALT, KC_SPC,                                  KC_ENT, KC_EQL, _______, KC_NUM, KC_CAPS
),
};

#if defined(ENCODER_MAP_ENABLE)
// const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    // [_BASE] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_MPRV, KC_MNXT) },
//     [_BASE] = { ENCODER_CCW_CW(QK_USER_28, QK_USER_29), ENCODER_CCW_CW(QK_USER_30, QK_USER_31) },
//     [_LOWER] = { ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______) },
//     [_RAISE] = { ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______) }
// };
#endif

#ifdef OLED_ENABLE

static const uint16_t SPLASH_DURATION_MS = 2500;

#include "transactions.h"

typedef struct {
    uint32_t oled_on;
} oled_state_m2s_t;
typedef struct {
    uint16_t keycode;
} lastkey_m2s_t;

typedef struct {
    eeprom_data_t data;
} config_sync_t;

typedef struct {    
    uint8_t left;
    uint8_t right;
} pressed_keys_m2s_t;

static bool g_oled_init_done = false;
static bool g_splash_active = true;
static bool g_splash_rendered = false;
static uint32_t g_splash_start_ms = 0;
static uint16_t g_last_keycode = KC_NO;

static uint32_t g_user_ontime;
static oled_state_m2s_t g_remote_oled_state = { true };
static uint32_t oled_last_sync = 0;
static inline pin_t get_charge_pump_enable_pin(void) {
    return GP20;
}

/*--------------------------------------------------
                Custom declarations
--------------------------------------------------*/
uint16_t unwrap_keycode(uint16_t kc) {
    if (kc >= QK_MOD_TAP && kc <= QK_MOD_TAP_MAX) {
        return QK_MOD_TAP_GET_TAP_KEYCODE(kc);
    }
    if (kc >= QK_LAYER_TAP && kc <= QK_LAYER_TAP_MAX) {
        return QK_LAYER_TAP_GET_TAP_KEYCODE(kc);
    }
    return kc;
}

void render_splash(void) {
    if (g_splash_rendered) {
        return;
    }
    oled_clear();
    oled_set_cursor(0, 0);
    oled_write_raw_P(STARTUP_BITMAP, sizeof(STARTUP_BITMAP));
    g_splash_rendered = true;
}

uint16_t change_left_encoder_layer(void) {
    if (timer_elapsed32(l_enc_timer) > 350) {
        if (l_enc_layer >= _EC_L_ZOOM) {
            l_enc_layer = _EC_L_VOLUME;
        } else {
            l_enc_layer++;
        }
    } else {
        switch (l_enc_layer) {
            case _EC_L_MEDIA:
                return KC_MPLY;
                break;
            
            default:
                return KC_MUTE;
                break;
        }   
    }

    return KC_TRANSPARENT;
}

/*--------------------------------------------------
                Implicit declarations
--------------------------------------------------*/
static void user_sync_oled_state_slave(uint8_t in_len, const void* in_data,
                                       uint8_t out_len, void* out_data) {
    if (in_len >= sizeof(oled_state_m2s_t)) {
        memcpy(&g_remote_oled_state, in_data, sizeof(oled_state_m2s_t));
    }
}

static void user_sync_lastkey_slave(uint8_t in_len, const void* in_data,
                                    uint8_t out_len, void* out_data) {
    if (in_len >= sizeof(lastkey_m2s_t)) {
        const lastkey_m2s_t* p = (const lastkey_m2s_t*)in_data;      
        g_last_keycode = p->keycode;
    }
}

static void user_sync_presses_slave(uint8_t in_len, const void* in_data,
                                    uint8_t out_len, void* out_data) {
    if (in_len >= sizeof(presses_m2s_t)) {
        presses_m2s_t remote_presses = {0,0};
        memcpy(&remote_presses, in_data, sizeof(presses_m2s_t));
        update_remote_presses(remote_presses);
    }
}

// static void user_sync_config_slave(uint8_t in_len, const void* in_data,
//                                    uint8_t out_len, void* out_data) {
//     if (in_len >= sizeof(config_sync_t)) {
//         const config_sync_t* p = (const config_sync_t*)in_data;
//         g_remote_data = p->data;
//         g_config_synced = true;
//     }
// }

void keyboard_post_init_user(void) {
    pin_t dsp_pen_pin = get_charge_pump_enable_pin();
    gpio_set_pin_output(dsp_pen_pin);
    gpio_write_pin_low(dsp_pen_pin);
    wait_ms(5);

    transaction_register_rpc(USER_SYNC_OLED_STATE, user_sync_oled_state_slave);
    transaction_register_rpc(USER_SYNC_LASTKEY, user_sync_lastkey_slave);
    transaction_register_rpc(USER_SYNC_PRESSES, user_sync_presses_slave);
    // transaction_register_rpc(USER_SYNC_CONFIG, user_sync_config_slave);

    if (!is_keyboard_master()) {
        wait_ms(90); // wait for master to be ready
    }
    
    init_globals();
    // storage_init();
    anim_bongocat_init(); 
    menu_init();
}

void housekeeping_task_user(void) {
    uint32_t curr_timer = timer_read32();

    if (is_keyboard_master()) {

        if (timer_elapsed32(oled_last_sync) > 50) {
            oled_state_m2s_t oled_state_pkt = { is_oled_on() };
            (void)transaction_rpc_send(
                USER_SYNC_OLED_STATE, sizeof(oled_state_pkt), &oled_state_pkt
            );
            oled_last_sync = curr_timer;
        }

        //Left ENCODER_RESET_TIMEOUT_USER
        uint32_t encsync = timer_elapsed32(l_enc_sync_time);    
        if (encsync > ENCODER_RESET_TIMEOUT_USER) {
            l_enc_layer = _EC_L_VOLUME;
            l_enc_sync_time = curr_timer;
        }
    }  
}

bool encoder_update_user(uint8_t index, bool clockwise) {
    const bool is_left_encoder = index == 0;

    if (is_left_encoder) {
        l_enc_sync_time = timer_read32();
        switch (l_enc_layer){
            case _EC_L_VOLUME:
                if (clockwise) {
                    tap_code(KC_VOLU);
                } else {
                    tap_code(KC_VOLD);
                }
                break;
            case _EC_L_MEDIA: 
                if (clockwise) {
                    tap_code(KC_MNXT);
                } else { 
                    tap_code(KC_MPRV);
                }
                break;
             case _EC_L_ZOOM: 
                if (clockwise) {
                    tap_code16(C(KC_PPLS));
                } else { 
                    tap_code16(C(KC_PMNS));
                }
                break;
            default:
                break;
        }
    } else {
        if (menu_is_active()) {            
            menu_encoder_rotate(clockwise);
            return false;
        } 

        if (clockwise) {
            tap_code(KC_RIGHT);
        } else {
            tap_code(KC_LEFT);
        }

    }
    return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        if (keycode == KC_F13 && layer_state_is(_LOWER)) {
            reset_keyboard();
            return false;
        } 
        if (keycode == KC_F14 && layer_state_is(_RAISE)) {
            reset_keyboard();
            return false;
        }
    }

    g_user_ontime = timer_read32();
    
    if (menu_check_keypress(keycode, record->event.pressed)) {
        //if the menu keypress has handled the event, finish the record_user
        return false;
    }

    uint8_t row = record->event.key.row;
    bool is_left_side = (row < MATRIX_ROWS / 2);

    if (record->event.pressed) {
        /*--------------------------------------
                         ENCODER
        ----------------------------------------*/
     
        switch (keycode) {
            case KC_F13:              
                l_enc_pressed = true;
                l_enc_timer = timer_read32();                
                break;
            default:
                break;
        }       
        /*--------------------------------------*/
        g_last_keycode = keycode;
        presses_m2s_t total_presses;
        if (is_left_side) {
            total_presses = update_pressed_key_counters('l', true);
        } else {
            total_presses = update_pressed_key_counters('r', true);
        }

        if (is_keyboard_master()) {
            lastkey_m2s_t keycode_pkt = { g_last_keycode };
            (void)transaction_rpc_send(USER_SYNC_LASTKEY, sizeof(keycode_pkt), &keycode_pkt);

            presses_m2s_t presses_pkt = { total_presses.left, total_presses.right };
            (void)transaction_rpc_send(USER_SYNC_PRESSES, sizeof(presses_pkt), &presses_pkt);            
        }
    } else {
        if (is_left_side) {
            update_pressed_key_counters('l', false);
        } else {
            update_pressed_key_counters('r', false);
        }

        /*--------------------------------------
                         ENCODER
        ----------------------------------------*/      
        switch (keycode) {
            case KC_F13: //Left encoder   
                if (!l_enc_pressed) {
                    break;
                } else {
                    l_enc_pressed = false;
                    uint16_t key_to_tap = change_left_encoder_layer();
                    if (key_to_tap != KC_TRANSPARENT) {
                        tap_code(key_to_tap);
                        return false;
                    }
                }                                
                break;            
            default:
                break;
        }       
        /*--------------------------------------*/        
    }
    return true;
}

void matrix_scan_user(void) {
    if (!is_keyboard_master() && !is_keyboard_left()) {
        return;
    }

    presses_m2s_t current_pressed = get_current_pressed();
    anim_bongocat_update(&current_pressed.left, &current_pressed.right, &g_user_ontime);
    menu_check_usertime();
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_270;
}

bool oled_post_init(void) {
    if (!g_oled_init_done) {
        // set OLED size for characters
        update_oled_limits();

        // enable charge pump
        pin_t dsp_pen_pin = get_charge_pump_enable_pin();
        gpio_write_pin_high(dsp_pen_pin);
        wait_ms(20);
        oled_clear();

        // start timer for splash screen
        g_splash_start_ms = timer_read32();
        g_splash_active = true;

        g_oled_init_done = true;
    }
    return false;
}

bool oled_task_user(void) {
    // perform custom initialisation once
    oled_post_init();    

    if (g_splash_active) {
        if (timer_elapsed32(g_splash_start_ms) > SPLASH_DURATION_MS) {
            g_splash_active = false;
            oled_clear();
            g_user_ontime = timer_read32();
        } else {
            render_splash();
        }
        return false;
    }

    if (is_keyboard_master()) {
        const uint32_t idle_time = timer_elapsed32(g_user_ontime);
        if (!is_oled_on()) {
            if (idle_time > OLED_TIMEOUT_USER) {
                return false; // stay off
            } else {
                oled_on();
            }
        } else {
            if (idle_time > OLED_TIMEOUT_USER) {
                oled_off();
                return false;
            }
        }
    } else {
        if (g_remote_oled_state.oled_on) {
            if (!is_oled_on()) {
                oled_on();
            }
        } else {
            if (is_oled_on()) {
                oled_clear();
                oled_off();
                return false;
            }
        }
    }

    if (is_keyboard_left()) {
        widgets_render(_SCR_LEFT);        
    } else {        
        widgets_render(_SCR_RIGHT);       
    }

    return false;
}

#endif // OLED_ENABLE
