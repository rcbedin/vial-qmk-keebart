#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t rgb_mode;
    uint8_t left_anim;
    uint8_t right_anim;
} menu_config_t;

typedef struct {
    uint8_t brightness;
    uint8_t speed;
} lighting_config_t;

typedef struct {
    uint8_t version;
    menu_config_t menu;
    lighting_config_t lighting;
} eeprom_data_t;

extern eeprom_data_t g_data;
extern eeprom_data_t g_remote_data;
extern uint32_t last_config_sync;
extern bool g_config_synced;

void storage_init(void);
void storage_save(void);
