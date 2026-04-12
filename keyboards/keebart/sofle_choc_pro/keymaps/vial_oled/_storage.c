#include "_storage.h"
#include "eeprom.h"
#include "print.h"

#define EEPROM_ADDR 0
#define EEPROM_VERSION 2

// #define EECONFIG_USER_DATA ((uint32_t*)EECONFIG_USER)
eeprom_data_t g_remote_data;
eeprom_data_t g_data;

uint32_t last_config_sync = 0;
bool g_config_synced = false;

static void storage_defaults(void) {
    g_data.version = EEPROM_VERSION;

    g_data.menu.rgb_mode = 0;
    g_data.menu.left_anim = 0;
    g_data.menu.right_anim = 0;

    g_data.lighting.brightness = 100;
    g_data.lighting.speed = 5;
}

void storage_save(void) {
    // uprintf("save rgb mode\n" );
    // uprintf("menu rgb: %u \n",g_data.menu.rgb_mode );
    eeprom_update_block(&g_data, (void*)EEPROM_ADDR, sizeof(g_data));
}

void storage_init(void) {
    // uprintf("start: storage\n" );
    eeprom_read_block(&g_data, (void*)EEPROM_ADDR, sizeof(g_data));

    // uprintf("read rgb mode: %u", g_data.menu.rgb_mode );

    if (g_data.version != EEPROM_VERSION) {
        // uprintf("fallback to defaults");
        storage_defaults();
        storage_save();
    }
}