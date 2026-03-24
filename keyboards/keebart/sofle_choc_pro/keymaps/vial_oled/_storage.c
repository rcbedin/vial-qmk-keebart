#include "_storage.h"
#include "eeprom.h"

#define EEPROM_ADDR 0
#define EEPROM_VERSION 1

// #define EECONFIG_USER_DATA ((uint32_t*)EECONFIG_USER)

eeprom_data_t g_data;

static void storage_defaults(void) {
    g_data.version = EEPROM_VERSION;

    g_data.menu.rgb_mode = 0;
    g_data.menu.left_anim = 0;
    g_data.menu.right_anim = 0;

    g_data.lighting.brightness = 100;
    g_data.lighting.speed = 5;
}

void storage_save(void) {
    eeprom_update_block(&g_data, (void*)EEPROM_ADDR, sizeof(g_data));
}

void storage_init(void) {
    eeprom_read_block(&g_data, (void*)EEPROM_ADDR, sizeof(g_data));

    if (g_data.version != EEPROM_VERSION) {
        storage_defaults();
        storage_save();
    }
}