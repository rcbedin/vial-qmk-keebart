#include "quantum.h"
#include "bongo_cat.h"


bool anim_bongo_cat_active = false;

static const char *const *current_bongo_anim_frames = BONGO_IDLE;

void anim_bongocat_init(void) {
    anim_bongo_cat_active = true; 
    //TODO: implement logic to load from storage
}

const char* anim_bongocat_get_frame(uint8_t row) {
   return (const char *)pgm_read_ptr(&current_bongo_anim_frames[row]);
}

void anim_bongocat_update(uint32_t *left_pressed, uint32_t *right_pressed, uint32_t *user_time) {
    if (!anim_bongo_cat_active) {
        return;
    } 

    bool timeout = timer_elapsed32((*user_time)) > 150;

    bool left_active = (*left_pressed) > 0;
    bool right_active = (*right_pressed) > 0;        
  
    if (!timeout) {
        if (left_active && right_active) {
            current_bongo_anim_frames = BONGO_BUSY;

        } else if (left_active) {
            current_bongo_anim_frames = BONGO_LEFT_BUSY;

        } else if (right_active) {
            current_bongo_anim_frames = BONGO_RIGHT_BUSY;
        }

    } else {
        if (left_active && right_active) {
            current_bongo_anim_frames = BONGO_BUSY;

        } else if (left_active) {
            current_bongo_anim_frames = BONGO_LEFT_BUSY;

        } else if (right_active) {
            current_bongo_anim_frames = BONGO_RIGHT_BUSY;

        } else {
            current_bongo_anim_frames = BONGO_IDLE;
        }
    }
}