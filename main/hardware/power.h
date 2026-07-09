#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the power management subsystem (AXP2101 or basic ADC).
 *        For AXP2101, this must be called early in boot to enable LDOs/DCDCs
 *        for other peripherals (Display, IMU, etc.)
 * 
 * @return ESP_OK on success
 */
esp_err_t mimi_power_init(void);

/**
 * @brief Get the current battery percentage
 * 
 * @return Battery percentage (0-100)
 */
uint8_t mimi_power_get_battery_percent(void);

/**
 * @brief Check if the device is currently charging
 * 
 * @return true if charging, false otherwise
 */
bool mimi_power_is_charging(void);

/**
 * @brief Get battery voltage in millivolts
 * 
 * @return Voltage in mV
 */
uint16_t mimi_power_get_battery_voltage_mv(void);

#ifdef __cplusplus
}
#endif
