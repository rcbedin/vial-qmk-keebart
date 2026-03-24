#include "roboeyes.h"
#include "oled_driver.h"
#include "oled_menu.h"

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 64

// Only use bottom half
#define VIEW_Y_START 32
#define EYE_BASELINE_Y 42

#define EYE_W 18
#define EYE_H 18
#define PUPIL_W 6
#define PUPIL_H 6

// Eye structure
typedef struct {
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
} eye_t;

static eye_t left_eye;
static eye_t right_eye;

static int8_t pupil_dx = 0;
static int8_t pupil_dy = 0;

static uint8_t blink_state = 0;
static uint16_t blink_timer = 0;
static roboeyes_expression_t current_expr = ROBOEYES_NEUTRAL;

// Forward declarations
static void draw_rect(int x, int y, int w, int h);
static void draw_eye(eye_t *eye);
static void draw_pupil(eye_t *eye);
static void update_blink(void);
static void clear_bottom(void);

void roboeyes_init(void) {
    left_eye.w = EYE_W;
    left_eye.h = EYE_H;
    left_eye.x = 6;
    left_eye.y = EYE_BASELINE_Y;

    right_eye.w = EYE_W;
    right_eye.h = EYE_H;
    right_eye.x = SCREEN_WIDTH - right_eye.w - 6;
    right_eye.y = EYE_BASELINE_Y;
}

// Clear ONLY bottom half
static void clear_bottom(void) {
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        for (int j = VIEW_Y_START; j < SCREEN_HEIGHT; j++) {
            oled_write_pixel(i, j, false);
        }
    }
}

static void draw_rect(int x, int y, int w, int h) {
    for (int i = x; i < x + w; i++) {
        for (int j = y; j < y + h; j++) {
            if (i >= 0 && i < SCREEN_WIDTH &&
                j >= VIEW_Y_START && j < SCREEN_HEIGHT) {
                oled_write_pixel(i, j, true);
            }
        }
    }
}

static void update_blink(void) {
    blink_timer++;

    if (blink_timer > 200) blink_state = 1;
    if (blink_timer > 220) {
        blink_state = 0;
        blink_timer = 0;
    }
}

static void draw_eye(eye_t *eye) {
    int h = eye->h;
    int y = eye->y;

    if (blink_state) {
        h = 2;
        y = eye->y + (eye->h - h) / 2;
    }

    switch (current_expr) {
        case ROBOEYES_HAPPY:
            h -= 4;
            break;
        case ROBOEYES_ANGRY:
            y += 3;
            h -= 3;
            break;
        case ROBOEYES_SLEEPY:
            h = 4;
            break;
        default:
            break;
    }

    draw_rect(eye->x, y, eye->w, h);

    if (!blink_state) {
        draw_pupil(eye);
    }
}

static void draw_pupil(eye_t *eye) {
    int px = eye->x + eye->w/2 - PUPIL_W/2 + pupil_dx;
    int py = eye->y + eye->h/2 - PUPIL_H/2 + pupil_dy;

    if (px < eye->x) px = eye->x;
    if (px + PUPIL_W > eye->x + eye->w) px = eye->x + eye->w - PUPIL_W;
    if (py < VIEW_Y_START) py = VIEW_Y_START;
    if (py + PUPIL_H > eye->y + eye->h) py = eye->y + eye->h - PUPIL_H;

    draw_rect(px, py, PUPIL_W, PUPIL_H);
}

void roboeyes_render(void) {
    clear_bottom();

    update_blink();

    draw_eye(&left_eye);
    draw_eye(&right_eye);
}

void roboeyes_look(int8_t dx, int8_t dy) {
    pupil_dx += dx;
    pupil_dy += dy;

    if (pupil_dx > 5) pupil_dx = 5;
    if (pupil_dx < -5) pupil_dx = -5;
    if (pupil_dy > 5) pupil_dy = 5;
    if (pupil_dy < -5) pupil_dy = -5;
}

void roboeyes_set_expression(roboeyes_expression_t expr) {
    current_expr = expr;
}


// ================= USAGE IN keymap.c =================

/*
#include "roboeyes.h"

bool oled_task_user(void) {
    roboeyes_render();
    return false;
}

void keyboard_post_init_user(void) {
    roboeyes_init();
}
*/
