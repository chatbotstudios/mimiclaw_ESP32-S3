#pragma once

#include "esp_err.h"
#include <stdint.h>

#define QMI8658_I2C_ADDR 0x6B

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t imu_qmi8658_init(void);
esp_err_t imu_qmi8658_read_accel(float *x, float *y, float *z);
esp_err_t imu_qmi8658_read_gyro(float *x, float *y, float *z);
esp_err_t imu_qmi8658_read_temp(float *temp);

#ifdef __cplusplus
}
#endif
