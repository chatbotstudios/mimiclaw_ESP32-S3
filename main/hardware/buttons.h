#pragma once
#include "esp_err.h"

/**
 * Initialize physical buttons on GPIO 0 and 21.
 * BTN1 (GPIO 0): Refresh Dashboard
 * BTN2 (GPIO 21): Send Telegram Ping
 */
esp_err_t buttons_init(void);
