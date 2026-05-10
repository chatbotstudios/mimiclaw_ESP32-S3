#pragma once

#include "esp_err.h"

/**
 * Initialize the MicroSD card slot and mount the FAT filesystem.
 * Mounts the SD card to the "/sdcard" VFS path.
 */
esp_err_t sd_card_init(void);

/**
 * Unmount the SD card and free resources.
 */
void sd_card_deinit(void);
