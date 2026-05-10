#pragma once

#include "esp_err.h"
#include <stdbool.h>

/**
 * @brief Initialize the Bluetooth (NimBLE) stack
 */
esp_err_t bluetooth_init(void);

/**
 * @brief Deinitialize the Bluetooth stack
 */
esp_err_t bluetooth_deinit(void);

/**
 * @brief Start BLE Advertising
 */
esp_err_t bluetooth_advertise_start(void);

/**
 * @brief Stop BLE Advertising
 */
esp_err_t bluetooth_advertise_stop(void);

/**
 * @brief Check if Bluetooth stack is currently initialized
 */
bool bluetooth_is_enabled(void);

/**
 * @brief Scan for BLE devices
 *
 * @param duration_ms How long to scan
 * @param results_json Output buffer for JSON results
 * @param max_len Size of results buffer
 * @return esp_err_t
 */
esp_err_t bluetooth_ble_scan(int duration_ms, char *results_json,
                             size_t max_len);

/**
 * @brief Get local Bluetooth status and address
 */
esp_err_t bluetooth_get_info(char *info_json, size_t max_len);
