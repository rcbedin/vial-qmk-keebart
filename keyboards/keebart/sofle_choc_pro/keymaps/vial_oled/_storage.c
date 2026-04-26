#include "_storage.h"
#include "eeprom.h"
#include "print.h"

#define EEPROM_ADDR 0
#define EEPROM_VERSION 1

// #define EECONFIG_USER_DATA ((uint32_t*)EECONFIG_USER)
eeprom_data_t g_remote_data;


uint32_t last_config_sync = 0;
bool g_config_synced = false;

typedef union {
  uint32_t raw;
  struct {
    uint8_t version;
    uint8_t rgb_mode;
    uint8_t left_anim;
    uint8_t right_anim;
  };
} user_config_t;
user_config_t g_data;


void set_g_rgb(uint8_t value) {
    g_data.rgb_mode = value;
}

void storage_defaults(void) {
    // g_data.version = EEPROM_VERSION;
    g_data.version = EEPROM_VERSION;
    g_data.rgb_mode = 0;
    g_data.left_anim = 0;
    g_data.right_anim = 0;

    // g_data.rgb.brightness = 100;
    // g_data.lighting.speed = 5;
}

void storage_save(void) {
    uprintf("save rgb mode\n" );
    // uprintf("menu rgb: %u \n",g_data.menu.rgb_mode );
    // eeprom_update_block(&g_data, (void*)EEPROM_ADDR, sizeof(g_data));
    // eeconfig_update_user(user_config.raw)

}

void storage_init(void) {
    uprintf("start: storage\n" );
    // g_data.raw = eeprom_read_user();//(&g_data, (void*)EEPROM_ADDR, sizeof(g_data));

    // if (g_data.version != EEPROM_VERSION) {
    //     storage_defaults();
    //     storage_save();
    // }

    // uprintf("read rgb mode: %u\n", g_data.menu.rgb_mode );

    // if (g_data.version != EEPROM_VERSION) {
    //     uprintf("fallback to defaults");
    //     storage_defaults();
    //     storage_save();
    // }
}