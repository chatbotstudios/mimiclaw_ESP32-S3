#pragma once

#include "esp_err.h"
#include <stdbool.h>

typedef struct {
    float temperature; // in Celsius
    float humidity;    // in %
    // Additional generic sensor data (IMU)
    float accel_x;
    float accel_y;
    float accel_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    bool has_imu;
    bool has_env;
} mimi_sensor_data_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize all board sensors
 */
esp_err_t mimi_sensor_init(void);

/**
 * @brief Read data from the available sensors
 */
esp_err_t mimi_sensor_read(mimi_sensor_data_t *data);

#ifdef __cplusplus
}
#endif
