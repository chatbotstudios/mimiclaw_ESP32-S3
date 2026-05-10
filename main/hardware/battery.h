#ifndef BATTERY_H
#define BATTERY_H

#include "esp_err.h"

/**
 * @brief Initialize the battery sensing hardware (ADC)
 */
esp_err_t battery_init(void);

/**
 * @brief Get the current battery voltage in volts
 */
float battery_get_voltage(void);

/**
 * @brief Get the estimated battery percentage (0-100)
 */
int battery_get_percentage(void);

#endif // BATTERY_H
