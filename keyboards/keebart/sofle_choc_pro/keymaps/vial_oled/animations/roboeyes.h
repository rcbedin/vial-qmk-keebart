#pragma once

#include "quantum.h"

// Expressions
typedef enum {
    ROBOEYES_NEUTRAL = 0,
    ROBOEYES_HAPPY,
    ROBOEYES_ANGRY,
    ROBOEYES_SLEEPY
} roboeyes_expression_t;

void roboeyes_init(void);
void roboeyes_render(void);
void roboeyes_look(int8_t dx, int8_t dy);
void roboeyes_set_expression(roboeyes_expression_t expr);
