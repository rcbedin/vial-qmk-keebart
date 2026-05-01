// #include QMK_KEYBOARD_H
// #include "rgb_matrix.h"
// #include "lib/lib8tion/lib8tion.h"

// #define RGB_MATRIX_SPLASH_BG_VAL 16

// bool effect_runner_reactive_splash_bg(uint8_t start, effect_params_t* params, reactive_splash_f effect_func) {
//     RGB_MATRIX_USE_LIMITS(led_min, led_max);

//     uint16_t max_tick = 65535 / qadd8(rgb_matrix_config.speed, 1);

//     for (uint8_t i = led_min; i < led_max; i++) {
//         RGB_MATRIX_TEST_LED_FLAGS();

//         hsv_t hsv = rgb_matrix_config.hsv;
//         hsv.v = RGB_MATRIX_SPLASH_BG_VAL;

//         for (uint8_t j = start; j < g_last_hit_tracker.count; j++) {
//             int16_t dx = g_led_config.point[i].x - g_last_hit_tracker.x[j];
//             int16_t dy = g_led_config.point[i].y - g_last_hit_tracker.y[j];
//             uint8_t dist = sqrt16(dx * dx + dy * dy);
//             uint16_t tick = scale16by8(g_last_hit_tracker.tick[j], qadd8(rgb_matrix_config.speed, 1));

//             if (tick < max_tick) {
//                 hsv = effect_func(hsv, dx, dy, dist, tick);
//             }
//         }

//         RGB rgb = rgb_matrix_hsv_to_rgb(hsv);
//         rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
//     }

//     return led_max < DRIVER_LED_TOTAL;
// }

// bool rgb_matrix_multisplash_bg(effect_params_t* params) {
//     return effect_runner_reactive_splash_bg(0, params, &SPLASH_math);
// }

// RGB_MATRIX_EFFECT(MULTISPLASH_BG)

// bool MULTISPLASH_BG(effect_params_t* params) {
//     return rgb_matrix_multisplash_bg(params);
// }