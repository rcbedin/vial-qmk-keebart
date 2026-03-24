#include "quantum.h"
#include "_utils.h"
#include "_globals.h"

static const uint8_t OLED_WIDTH = OLED_DISPLAY_HEIGHT;

uint16_t get_current_dwpm(void) {
    const uint8_t wpm = get_current_wpm();
    uint16_t dwpm = (uint16_t)wpm * 10u;
    return dwpm;
}

uint8_t round_percentage(float x) {
    float f = x + 0.5f;
    uint8_t r = (uint8_t)f;
    if ((f - (float)r) == 0.0f && (r & 1)) {
        r--; // round half to even
    }
    return r;
}

void oled_blit_16x16_P(const char *icon, uint8_t x, uint8_t page) {
    for (uint8_t i = 0; i < 16; i++) {
        char top = pgm_read_byte(&icon[i]);         // column i, top 8 pixels
        char bot = pgm_read_byte(&icon[16 + i]);    // column i, bottom 8 pixels

        oled_write_raw_byte(top, page       * OLED_WIDTH + x + i);
        oled_write_raw_byte(bot, (page + 1) * OLED_WIDTH + x + i);
    }
}

void oled_blit_24x24_P(const char *icon, uint8_t x, uint8_t page) {
    for (uint8_t i = 0; i < 24; i++) {
        char top = pgm_read_byte(&icon[i]);         // column i, top 8 pixels
        char mid = pgm_read_byte(&icon[24 + i]);    // column i, middle 8 pixels
        char bot = pgm_read_byte(&icon[48 + i]);    // column i, bottom 8 pixels

        oled_write_raw_byte(top, page       * OLED_WIDTH + x + i);
        oled_write_raw_byte(mid, (page + 1) * OLED_WIDTH + x + i);
        oled_write_raw_byte(bot, (page + 2) * OLED_WIDTH + x + i);
    }
}

void oled_print_right_aligned(const char *text, const uint8_t width) {
    uint8_t len = strlen(text);
    uint8_t pad = (len < width) ? (width - len) : 0;
    for (uint8_t i = 0; i < pad; i++) {
        oled_write_P(PSTR(" "), false);
    }
    oled_write(text, false);
}
