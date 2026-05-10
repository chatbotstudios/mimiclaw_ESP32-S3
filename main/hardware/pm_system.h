#ifndef PM_SYSTEM_H
#define PM_SYSTEM_H

#include "esp_err.h"

typedef enum {
    MIMI_PWR_BALANCED = 0,
    MIMI_PWR_PERFORMANCE
} mimi_pwr_mode_t;

/**
 * @brief Initialize power management
 */
esp_err_t pm_system_init(void);

/**
 * @brief Set the power mode
 */
esp_err_t pm_system_set_mode(mimi_pwr_mode_t mode);

/**
 * @brief Get the current power mode
 */
mimi_pwr_mode_t pm_system_get_mode(void);

/**
 * @brief Enter deep sleep for a specified duration
 * @param ms Duration in milliseconds
 */
void pm_system_deep_sleep(uint64_t ms);

#endif // PM_SYSTEM_H
