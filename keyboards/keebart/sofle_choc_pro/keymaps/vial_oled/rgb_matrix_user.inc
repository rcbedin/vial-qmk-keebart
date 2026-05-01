// !!! DO NOT ADD #pragma once !!! //

RGB_MATRIX_EFFECT(MULTISPLASH_BG)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

#    define RGB_MATRIX_MULTISPLASH_BG_VAL 16

static hsv_t multisplash_bg_math(hsv_t hsv, int16_t dx, int16_t dy, uint8_t dist, uint16_t tick) {
    uint16_t effect = tick - dist;

    if (effect > 255) {
        effect = 255;
    }

    // hsv.v += effect;
    hsv.v = qadd8(hsv.v, 255 - effect);

    return hsv;
}

static bool MULTISPLASH_BG(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    uint8_t count = g_last_hit_tracker.count;
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        hsv_t hsv = rgb_matrix_config.hsv;
        hsv.v     = 15;
        for (uint8_t j = 0; j < count; j++) {
            int16_t  dx   = g_led_config.point[i].x - g_last_hit_tracker.x[j];
            int16_t  dy   = g_led_config.point[i].y - g_last_hit_tracker.y[j];
            uint8_t  dist = sqrt16(dx * dx + dy * dy);
            uint16_t tick = scale16by8(g_last_hit_tracker.tick[j], qadd8(rgb_matrix_config.speed, 1));
            hsv           = multisplash_bg_math(hsv, dx, dy, dist, tick);
        }
        hsv.v     = scale8(hsv.v, rgb_matrix_config.hsv.v);
        rgb_t rgb = rgb_matrix_hsv_to_rgb(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return rgb_matrix_check_finished_leds(led_max);
}

#endif